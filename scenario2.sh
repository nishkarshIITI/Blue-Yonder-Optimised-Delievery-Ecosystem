echo "DroneID,Day,Time (in Seconds),X,Y,Z,Activity,Payload Weight (W) in Kgs,Speed (m/s),mAH Consumed,Energy Cost (c X mAH)" > ./OutputData/Scenario2/DronePath1B.csv
g++ main.cpp
./a.out ./InputData/Scenario2/Parameters.csv ./InputData/Scenario2/Demand_Day1.csv ./OutputData/Scenario2/DronePath1B.csv "Day 1" 1
./a.out ./InputData/Scenario2/Parameters.csv ./InputData/Scenario2/Demand_Day2.csv ./OutputData/Scenario2/DronePath1B.csv "Day 2" 1