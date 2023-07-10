#!/usr/bin/env python
# coding: utf-8

# imports 
import pandas as pd
import numpy as np

filename = './OutputData/Scenario2/DronePath1B.csv'
out_filename = './OutputData/Scenario2/CostReport1B.csv'

# reading drone path file 
data = pd.read_csv(filename)

# created a set of unique drones
DroneIDs = sorted(set(data['DroneID'].to_list()))
# all the days goes here
Days = ["Day 1", "Day 2", "Day 3"]

# variable cost per drone
variable_cost = {"D1":5, "D2":8, "D3":13, "D4":15, "D5":20, "D6":25}

# required columns in Cost Report file
columns = ['Drone ID', 
           'FLIGHT TIME (in sec)(Day 1)', 
           'FLIGHT TIME (in sec) (Day 2)', 
           'FLIGHT TIME (in sec) (Day 3)', 
           'RESTING/ WAITING TIME  (in sec) (Day 1)', 
           'RESTING/ WAITING TIME  (in sec) (Day 2)', 
           'RESTING/ WAITING TIME  (in sec) (Day 3)',
           'CHARGING TIME  (in sec) (Day 1)',
           'CHARGING TIME  (in sec) (Day 2)',
           'CHARGING TIME  (in sec) (Day 3)',
           'MAINTENANCE VARIABLE COST ($) (Day 1)',
           'MAINTENANCE VARIABLE COST ($) (Day 2)',
           'MAINTENANCE VARIABLE COST ($) (Day 3)',
           'ENERGY COST ($) (Day 1)',
           'ENERGY COST ($) (Day 2)',
           'ENERGY COST ($) (Day 3)']

# creating data frame for cost report with required columns
cost_report = pd.DataFrame(columns = columns)

# replacing string values to ease calculations
data = data.replace("-", np.nan)
data['Energy Cost (c X mAH)'] = data['Energy Cost (c X mAH)'].astype(float).fillna(0.0)

# temp variable to see total costs
output = {"Day 1":{"E":0, "M":0}, "Day 2":{"E":0, "M":0}, "Day 3":{"E":0, "M":0}}

# filling required data in cost_report
for DroneID in DroneIDs:
    flight_times = list()
    rest_wait_times = list()
    charging_times = list()
    maintenance_variables = list()
    energy_costs = list()
    
    result = list()

    for Day in Days:
        tmp1 = data[data['DroneID'] == DroneID]
        tmp2 = tmp1[tmp1['Day'] == Day]
        
        flight_time = 0
    
        # calc flight time
        if len(tmp2.index) != 0:
            flight_time = max(tmp2['Time (in Seconds)']) - min(tmp2['Time (in Seconds)'])+1
        details = tmp2.apply(lambda x : True
                    if (x['Activity'].startswith("R-WH") or x['Activity'].startswith("R-RS") or x['Activity'].startswith("D") or x['Activity'].startswith("PU-WH") ) else False, axis = 1)

        # Count number of True in the series
        rest_wait_time = len(details[details == True].index)

        details = tmp2.apply(lambda x : True
                    if (x['Activity'].startswith("C-WH") or x['Activity'].startswith("C-RS") ) else False, axis = 1)

        charging_time = len(details[details == True].index)

        energy_cost = sum(tmp2['Energy Cost (c X mAH)'])

        maintenance_variable = variable_cost[DroneID[:2]] * flight_time/3600
        
        output[Day]['E'] += energy_cost
        output[Day]['M'] += maintenance_variable

        # appending to list all the required values
        flight_times.append(flight_time)
        rest_wait_times.append(rest_wait_time)
        charging_times.append(charging_time)
        maintenance_variables.append(maintenance_variable)
        energy_costs.append(energy_cost)
        
    result.append(DroneID)
    
    # adding current row to temp list 
    for l in [flight_times, rest_wait_times, charging_times, maintenance_variables, energy_costs]:
        for i in range(3):
            result.append(l[i])

    # appending to data frame
    cost_report.loc[len(cost_report.index)] = result

print(output)

# saving the csv file
cost_report.to_csv(out_filename)

