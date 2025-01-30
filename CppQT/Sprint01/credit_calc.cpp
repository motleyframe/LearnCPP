#include <iostream>
#include <cmath>
using namespace std::string_literals;

void Print(const int& month, const double& payment,
           const double& interest, const double& debt) {
    std::cout <<"Месяц: "s<<month<<
                " Платёж: "s<<payment<<
                " Проценты: "s<<interest<<
                " Долг: "s<<debt<<std::endl;
}

int main() {
    int s,t;
    double k;
    std::cin>>s>>k>>t;

    const double m = k/1200;
    const double power = std::pow((1+m),t);
    const double a = m*power / (power-1);
    const double x = s*a;
    double debt = s;

    for(int i=1; i<=t; ++i) {
        const double p=debt*m;
        const double f=x-p;
        Print(i,x,p,f);
        debt -= f;
    }

    return 0;
}
