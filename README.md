# BLUE YONDER'S NEXT-GEN OPTIMIZED DELIVERY ECOSYSTEM

**Our solution takes care of the base solution and all three complexity levels (1 + 2 + 3)**

**In all the 3 scenarios, we are able to deliver all the demands.**

## Input

Input data is stored in `InputData` folder. (Same as the one provided on drive)

## Output

`OutputData` contains 3 folders for each scenario: Scenario1, Scenario2 and Scenario3

`Scenario1` contains 2 output files `CostReport.csv` and `DronePath.csv` containing cost report and drone path respectively. Since there were no delivery failures in this scenario, there are only 2 output files.

`Scenario2` contains 4 output files `CostReport1A.csv`, `CostReport1B.csv`, `DronePath1A.csv` and `DronePath1B.csv` containing cost reports and drone paths respectively. Since Complexity 3 is being handled, CostReport1A and DronePath1A will be the original plan whereas CostReport1B and DronePath1B will be the replan after cancellation.

`Scenario3` contains 4 output files `CostReport1A.csv`, `CostReport1B.csv`, `DronePath1A.csv` and `DronePath1B.csv` containing cost reports and drone paths respectively. Since Complexity 3 is being handled, CostReport1A and DronePath1A will be the original plan whereas CostReport1B and DronePath1B will be the replan after cancellation.

## Implementation

`main.cpp` contains the logic and implementation of our algorithm.

`DronePath2CostReport.py` contains code to fill Cost Reports from Drone Path output.

`scenario1.sh`, `scenario2.sh` and `scenario3.sh` contains bash scripts for automated code running. 





