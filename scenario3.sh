echo "DroneID,Day,Time (in Seconds),X,Y,Z,Activity,Payload Weight (W) in Kgs,Speed (m/s),mAH Consumed,Energy Cost (c X mAH)" > ./OutputData/Scenario3/DronePath1B.csv
g++ main.cpp
./a.out ./InputData/Scenario3/Parameters.csv ./InputData/Scenario3/Demand_Day1.csv ./OutputData/Scenario3/DronePath1B.csv "Day 1" 1
./a.out ./InputData/Scenario3/Parameters.csv ./InputData/Scenario3/Demand_Day2.csv ./OutputData/Scenario3/DronePath1B.csv "Day 2" 1
./a.out ./InputData/Scenario3/Parameters.csv ./InputData/Scenario3/Demand_Day3.csv ./OutputData/Scenario3/DronePath1B.csv "Day 3" 1