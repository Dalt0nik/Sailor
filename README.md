# Sailor

## About

"Sailor" - a financial management tool designed for investment tracking. This project provides you with the ability to monitor your investment portfolio conveniently and efficiently, depicting not only the growth chart of your stocks but also the overall progress of your account balance.

## External tools and libraries

### Stock API
We utilize Alpha Vantage as an API to access financial data and analysis tools for our "Sailor" project.

### C++ Requests: Curl for People 
It's a simple wrapper around libcurl library 
https://github.com/whoshuu/cpr.git

### Chart library
To be chosen.

## How to build
 1. Run CMake to generate build/out directory
 2. Build executable, depending on how you utilize CMake
 3. Create ```secrets.txt``` next to ```Sailor``` executable
 4. Generate Alpha Vantage key here: https://www.alphavantage.co/support/#api-key and put it in ```secrets.txt```
 5. Run ```Sailor```
 
## Team
Artiom Hovhannisyan, Evgenij Shapovalov.
Teamname: lonely sailors