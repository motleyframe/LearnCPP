#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std::string_literals;

std::pair<bool, double> CalcMedian(std::vector<double> samples) {
    if(samples.empty())
        return std::make_pair(false,0.0);

    size_t sz = samples.size();
    if(sz==1)
        return std::make_pair(true,samples[0]);

    std::nth_element(samples.begin(),
                     samples.begin()+sz/2, 
                     samples.end());

    double median = sz&1 ? samples[sz/2] : (samples[sz/2] + samples[sz/2-1])/2;
    
    return std::make_pair(true,median);
}

int main() {
    int size;
    std::cin >> size;

    std::vector<double> samples;
    for (int i = 0; i < size; ++i) {
        double sample;
        std::cin >> sample;
        samples.push_back(sample);
    }

    std::pair<bool, double> result = CalcMedian(samples);
    if (result.first) {
        std::cout << result.second << std::endl;
    } else {
        std::cout << "Empty vector\n"s;
    }
    return 0;
}
