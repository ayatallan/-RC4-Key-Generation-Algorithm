#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <bitset>  

using namespace std;

// Function to initialize the RC4 state array using the provided key
void RC4Initialize(unsigned char state[], const string& key) {
    unsigned char S[256];
    for (int i = 0; i < 256; i++) {
        S[i] = static_cast<unsigned char>(i);
    }

    int j = 0;
    for (int i = 0; i <= 255; i++) {
        j = (j + S[i] + key[i % key.size()]) % 256;
        swap(S[i], S[j]); // Swap S[i] and S[j]
    }

    for (int i = 0; i < 256; i++) {
        state[i] = S[i];
    }
}

// Function to generate a random 128-bit key with printable ASCII characters
string GenerateRandom128BitKey() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distribution(33, 126); // Printable ASCII characters range

    string key;
    for (int i = 0; i < 16; i++) {
        key.push_back(static_cast<char>(distribution(gen)));
    }

    return key;
}


// Function to calculate the Hamming distance between two binary strings
double HammingDistance(const string& str1, const string& str2) {
    if (str1.length() != str2.length()) {
        throw invalid_argument("Strings must be of the same length for Hamming distance calculation.");
    }

    double distance = 0.0;
    for (size_t i = 0; i < str1.length(); i++) {
        if (str1[i] != str2[i]) {
            distance += 1.0; // Increment the distance if bits differ
        }
    }

    return distance / str1.length(); // Divide the result by the string length
}
int main() {
    unsigned char state[256];
    string key;
    int option;
    cout << "Choose an option:" << endl;
    cout << "1. Enter a 128-bit key for RC4" << endl;
    cout << "2. Generate a random 128-bit key" << endl;
    cin >> option;

    if (option == 1) {
        cout << "Enter a 128-bit key for RC4 (16 characters): ";
        cin >> key;

        if (key.size() != 16) {
            cout << "Error: The key must be 16 characters (128 bits) long." << endl;
            return 1;
        }

        RC4Initialize(state, key);
        cout << "RC4 state array has been initialized with the provided key." << endl << endl;
    } else if (option == 2) {
        key = GenerateRandom128BitKey();
        cout << "Random 128-bit key generated: " << key << endl;

        // Initialize the RC4 state array with the random key
        RC4Initialize(state, key);
        cout << "RC4 state array has been initialized with the random key." << endl << endl;
    } else {
        cout << "Invalid option. Please choose option 1 or 2." << endl;
        return 1;
    }

// Perform the old test
int count = 0;
for (int i = 0; i < 10000; i++) {
    // Generate the first byte
    int firstByte = state[i % 256];

    // Generate the second byte
    int secondByte = state[(i + 1) % 256];

    // Check if the second byte is zero
    if (secondByte == 0) {
        count++;
    }
}


    // Calculate the probability of the second byte being zero
    double probability = static_cast<double>(count) / 10000;

    // Check if the probability is approximately 2/256
    cout << "Test 1 , The probability of the second byte being zero , RESULT :" << endl;
    cout << " the probability is : "<<probability<<" ~ = "<<"2/256 or 1/128"<<endl;
    if (probability < 2.0 / 256) {
        cout << " The probability of the second byte being zero is low." << endl;
    } else {
        cout << " The probability of the second byte being zero is not low." << endl;
    }
    cout << endl << endl;

    // Perform the new test to demonstrate (0,0) pair weakness
    int count00 = 0;
    for (int i = 0; i < 1000000; i++) {
        // Generate two consecutive bytes
        int firstByte = state[i % 256];
        int secondByte = state[(i + 1) % 256];

        // Check if it's the (0,0) pair
        if (firstByte == 0 && secondByte == 0) {
            count00++;
        }
    }

    // Calculate the probability of the (0,0) pair
    double probability00 = static_cast<double>(count00) / 1000000;

    // Expected probability: 2/(256^2) + 1/(256^3)
    double expectedProbability = 2.0 / (256 * 256) + 1.0 / (256 * 256 * 256);

    cout << "Test 2, Probability of (0,0) pair ,RESULT :" << endl;
    cout << "Expected probability: " << expectedProbability << endl;
    cout << endl;

   // if (probability00 == expectedProbability) {
  //      cout << "Pass: The probability of the (0,0) pair matches the expected value." << endl;
  //  } else {
   //     cout << "Fail: The probability of the (0,0) pair does not match the expected value." << endl;
 //   }
 //   cout << endl << endl;

    // Perform the new test to demonstrate related key attacks
    string originalKey = key; // Store the original key

    // Generate a new key with one bit difference
    key[key.size() - 1] ^= 1; // Flip the last bit

    // Initialize the RC4 state array with the new key
    RC4Initialize(state, key);

     // Generate 1000 bytes for each key and calculate the Hamming distance
    double totalHammingDistance = 0.0; // Total Hamming distance
    for (int i = 0; i < 1000; i++) {
        // Generate the first byte with the original key
        int firstByteOriginal = state[i % 256];

        // Generate the first byte with the modified key
        key[key.size() - 1] ^= 1; // Flip the last bit
        RC4Initialize(state, key);
        int firstByteModified = state[i % 256];

        // Convert the bytes to binary strings
        string binaryOriginal = bitset<8>(firstByteOriginal).to_string();
        string binaryModified = bitset<8>(firstByteModified).to_string();

        // Calculate Hamming distance
        totalHammingDistance += HammingDistance(binaryOriginal, binaryModified);
    }

    // Calculate the average Hamming distance
    double averageHammingDistance = totalHammingDistance / 1000;
    string str1, str2;

    // Check if the average Hamming distance is significantly different from the expected value
    cout << "Related Key Attack Test Result:" << endl;
    cout << "Average Hamming Distance: " << averageHammingDistance << endl;
    cout << "Enter 2  string to calculate Hamming Distance : " << endl;
    cin >> str1 >> str2;
    cout << "Hamming Distance (Example): " << HammingDistance(str1, str2) << endl;  // Example Hamming Distance
    cout << endl;

    // Define a lower expected threshold for a related key attack
    double expectedThreshold = 0.5; // A lower threshold

    // if (averageHammingDistance < expectedThreshold) {
    //     cout << "Fail: The average Hamming distance is below the expected threshold, indicating a related key attack." << endl;
    // } else {
    //     cout << "Pass: The average Hamming distance is not below the expected threshold." << endl;
    // }
    // cout << endl << endl;

    return 0;
}
