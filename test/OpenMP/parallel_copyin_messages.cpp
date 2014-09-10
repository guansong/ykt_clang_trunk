// RUN: %clang_cc1 -triple x86_64-apple-macos10.7.0 -verify -fopenmp -ferror-limit 100 %s

void foo() {
}

bool foobool(int argc) {
  return argc;
}

struct S1; // expected-note {{declared here}}
class S2 {
  mutable int a;
public:
  S2():a(0) { }
  S2 & operator =(S2 &s2) { return *this; }
};
class S3 {
  int a;
public:
  S3():a(0) { }
  S3 &operator =(S3 &s3) { return *this; }
};
class S4 { // expected-note {{'S4' declared here}}
  int a;
  S4();
  S4 &operator =(const S4 &s4);
public:
  S4(int v):a(v) { }
};
class S5 { // expected-note {{'S5' declared here}}
  int a;
  S5():a(0) {}
  S5 &operator =(const S5 &s5) { return *this; }
public:
  S5(int v):a(v) { }
};

S2 k;
S3 h;
S4 l(3); // expected-note {{'l' defined here}}
S5 m(4); // expected-note {{'m' defined here}}
#pragma omp threadprivate(h, k, l, m)

int main(int argc, char **argv) {
  int i;
  #pragma omp parallel copyin // expected-error {{expected '(' after 'copyin'}} expected-error {{expected expression}}
  #pragma omp parallel copyin ( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp parallel copyin () // expected-error {{expected expression}}
  #pragma omp parallel copyin (k // expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp parallel copyin (h, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp parallel copyin (argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  #pragma omp parallel copyin (l) // expected-error {{copyin variable must have an accessible, unambiguous copy assignment operator}}
  #pragma omp parallel copyin (S1) // expected-error {{'S1' does not refer to a value}}
  #pragma omp parallel copyin (argv[1]) // expected-error {{expected variable name}}
  #pragma omp parallel copyin(i) // expected-error {{copyin variable must be threadprivate}}
  #pragma omp parallel copyin(m) // expected-error {{copyin variable must have an accessible, unambiguous copy assignment operator}}
  foo();

  return 0;
}
