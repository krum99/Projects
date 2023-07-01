#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

//UPDATE:
//1. Create class Observation (or other name), which contains the data for one person and it's result
//2. Data validation to be added
//3. Check for efficiency of the algorithm

class Classifier {
private:
    vector<vector<double>> data;
    vector<double> result;
    vector<double> coefficients;

public:
    void loadTrainingData(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        double value;
        while (file >> value) {
            vector<double> row;
            for (int i = 0; i < 6; ++i) {
                file >> value;
                row.push_back(value);
            }
            data.push_back(row);

            file >> value;
            result.push_back(value);
        }

        file.close();
    }

    void train() {
        const double learningRate = 0.01;
        const int numIterations = 1000;

        int numFeatures = data[0].size();
        coefficients.resize(numFeatures + 1, 0.0); // Initialize coefficients to zero
        vector<double> gradients(numFeatures + 1, 0.0);

        for (int iteration = 0; iteration < numIterations; ++iteration) {
            for (int i = 0; i < data.size(); ++i) {
                double prediction = predict(data[i]);
                double error = prediction - result[i];

                gradients[0] += error;
                for (int j = 1; j <= numFeatures; ++j) {
                    gradients[j] += error * data[i][j - 1];
                }
            }

            for (int j = 0; j <= numFeatures; ++j) {
                coefficients[j] -= learningRate * gradients[j];
                gradients[j] = 0.0;
            }
        }
    }

    double predict(const vector<double>& features) {
        double prediction = coefficients[0];
        for (int i = 1; i < coefficients.size(); ++i) {
            prediction += coefficients[i] * features[i - 1];
        }
        //cout<<coefficients.size()<<endl;
        return sigmoid(prediction);
    }

    double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }
};

int main() {
    Classifier classifier;
    classifier.loadTrainingData("credit_data.txt");
    classifier.train();

    //data meaning (as it follows): rating,income,experience,balance,outcome,age
    vector<double> newCreditFeatures = {1.2, 1.8, 2.5, 1.5, -1.6, 0};
    double prediction = classifier.predict(newCreditFeatures);
    if(prediction >= 0.5)
    {
        cout<<"Approved! :)";
    }
    else
    {
        cout<<"Not approved! :(";
    }
    //cout << "Credit approved? : " << prediction << endl;

    return 0;
}
