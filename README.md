# Order Matching Engine for a Stock Exchange
A Simple C++ Implementation of an Order Matching Engine.
It demonstrates the core functionalities of an exchange's matching system - handling orders, maintaing an order book and generating trades.

---

## Setup :

### Dependencies and Configuration :
- Please make sure you have Docker installed on your system.
- All the necessary dependencies for running the application are baked into the container image by using a Dockerfile. Please see the Dockerfile for more information. These dependencies are :
  - C++ Compiler
  - GoogleTest
- `make test` generates test artifact during Docker build time
- `make build` also generates build artifact during Docker build time
- Additionally, there is `trading_hours.ini` file that holds the open and close time for Stock Exchange. The application will choose to place orders on the basis of contents in this file.

### Steps to run :
- To run the application, use `docker run -it stockmarket make run`
- To test the application, use `docker run -it stockmarket make test`
- You can also manually, run the container in interactive mode and then run the executable inside the shell. To do this :
  - `docker run -it stockmarket`
  - `bin/stock_market trading_hours.ini`
