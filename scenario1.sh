echo "DroneID,Day,Time (in Seconds),X,Y,Z,Activity,Payload Weight (W) in Kgs,Speed (m/s),mAH Consumed,Energy Cost (c X mAH)" > ./OutputData/Scenario1/DronePath.csv
g++ main.cpp
./a.out ./InputData/Scenario1/Parameters.csv ./InputData/Scenario1/Demand.csv ./OutputData/Scenario1/DronePath.csv "Day 1" 0