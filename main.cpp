#include <bits/stdc++.h>
using namespace std;

#define ld long double
#define ll long long
#define INF (long long)1e9

// constant global variables
constexpr int Total_Item_Types = 5;
constexpr int Warehouse_Current = 5;
constexpr int Recharge_Point_Current = 3;
constexpr int Warehouse_Count = 3;
constexpr int Recharge_Point_Count = 5;
constexpr int Drone_Type_Count = 6;
constexpr int NoFlyZone_Count = 1000;
constexpr int Max_Demand_Count = 1000;

// Input Parameters from terminal
string parameter_file;
string demand_file;
string day;
string output_file;
bool failure;

// Struct to store the (x, y, z) coordinates of a position.
struct Coordinate{
	int x,y,z;

	Coordinate() {
		x = 0; y = 0; z = 0;
	}
};

// Struct to store NoFlyZone details
struct NoFlyZone {
	int x_cords[8];
	int y_cords[8];
	int z_cords[8];
	Coordinate bottom_left, top_right;
};

// Struct to store time HH:MM:SS.
struct Timing{
	int hr,min,sec;
	int to_sec;	// to_sec converts HH:MM:SS format to total seconds from 00:00:00

	// converts HH:MM:SS -> to_sec
	void convert_to_sec(){
		to_sec = hr * 3600 + min * 60 + sec;
	}

	// converts to_sec -> HH:MM:SS
	void convert_to_time(){
		int tmp = to_sec;
		hr = tmp / 3600;
		tmp -= hr * 3600;
		min = tmp / 60;
		tmp -= min * 60;
		sec = tmp;
	}

};

// Struct to store the Drone details
struct Drone{
	int count;
	int battery_capacity;
	ld base_weight;
	ld payload_capacity_weight;
	ld payload_capacity_volume;
	int max_slots;
	int fixed_cost; // Maintenance Fixed Cost (per day)
	int variable_cost; // Maintenance Variable Cost (per hour of flight time)
	int P, Q, M; // speed profiles
	ld A, B, C; // enery consumption profilerrent_coordinate;

	// Stores information for each of the drone of this DroneType
	vector<Timing> last_demand;
	vector<ld> current_battery;
    vector<int> on_warehouse;
	vector<vector<vector<string>>> status;
	
	// Initializes the drone parameters
	void Initialise(int battery_capacity,ld base_weight,ld payload_capacity_weight,ld payload_capacity_volume,int max_slots){
		this->battery_capacity = battery_capacity;
		this->base_weight = base_weight;
		this->payload_capacity_weight = payload_capacity_weight;
		this->payload_capacity_volume = payload_capacity_volume;
		this->max_slots = max_slots;
	}
};

// struct to store the Item details
struct Item{
	int weight, length, breadth, height;
	int volume;
	void Initialise(int weight, int length, int breadth, int height){
		this->weight = weight;
		this->length = length;
		this->breadth = breadth;
		this->height = height;
	}
};

// struct to store the Warehouse details
struct Warehouse{
	Coordinate Position;
};

// struct to store the Recharge Station details
struct Recharge_Point{
	Coordinate Position;
};

// struct to store the Demand details
struct Demand{
	string demand_id;
	Coordinate Delievery_Point;
	int item_id;
	int warehouse_id;
	int Day;
	Timing Delievery_From,Delievery_To;
	bool Delievery_Failure;
};

Warehouse Warehouses[Warehouse_Count];                 // array to store all the warehouses from the input
Recharge_Point Recharge_Points[Recharge_Point_Count];  // array to store all the recharch points from the input
Drone Drones[Drone_Type_Count];                        // array to store all the drones and their details based on count and attributes from the input
NoFlyZone NoFlyZones[NoFlyZone_Count];                 // array to store no fly zones from the input
Item Items[Total_Item_Types];                          // array to store item details for each type
Demand Demands[Max_Demand_Count];                      // array to store demand details from the input

int max_speed;
ld Cost_Of_Charging;
int demand_count;
int demand_failed = 0;

// Helper function to Initialize the static details of Items
void Initialise_Items(){
	Items[0].Initialise(1,5,8,5);
	Items[1].Initialise(6,5,10,8);
	Items[2].Initialise(4,5,10,15);
	Items[3].Initialise(2,15,10,8);
	Items[4].Initialise(5,20,15,10);
	for(int i = 0; i < Total_Item_Types; i++){
		Items[i].volume=Items[i].length*Items[i].breadth*Items[i].height;
	}
}

// Helper function to Initialize the static details of drone
void Initialise_Drone_Details(){
	Drones[0].Initialise(2000,2.0,5,200,1);
	Drones[1].Initialise(2500,2.5,6,500,1);
	Drones[2].Initialise(3000,3.0,7,1000,2);
	Drones[3].Initialise(4000,3.5,8,2000,2);
	Drones[4].Initialise(5000,4.0,9,3000,2);
	Drones[5].Initialise(10000,5.0,10,5000,4);
}

// Helper function to take input from the Parameter file
void Take_Input_Parameters(){
	vector<string> row;
	fstream file (parameter_file, ios::in);
	string line, word;
	int maxspeed,cost;
	string first_string,last_string;
	while(getline(file, line))
	{
		row.clear();
		stringstream str(line);
		while(getline(str, word, ',')){
			row.push_back(word);
		}
		first_string = row[0];
		if(first_string.substr(0,8) == "MaxSpeed") {
			max_speed = stoi(row[1]);
			continue;
		}
		if(first_string.substr(0,4) == "Cost"){
			Cost_Of_Charging = stod(row[1]);
			continue;
		}
		last_string = row.back();
		if(last_string == "Noflyzone"){
			int num;
			int sz = first_string.size();
			int cord_no;
			num = stoi(first_string.substr(1,sz-2));
			cord_no = int(first_string.back()-'0');
			if(first_string[0]=='X') NoFlyZones[num].x_cords[cord_no] = stoi(row[1]);
			if(first_string[0]=='Y') NoFlyZones[num].y_cords[cord_no] = stoi(row[1]);
			if(first_string[0]=='Z') NoFlyZones[num].z_cords[cord_no] = stoi(row[1]);
		}
		else if(last_string.substr(0,8) == "Recharge"){
			int ind = first_string[2]-'1';
			if(first_string[1] == 'X'){
				Recharge_Points[ind].Position.x = stoi(row[1]);
			}
			if(first_string[1] == 'Y'){
				Recharge_Points[ind].Position.y = stoi(row[1]);
			}
		}
		else if(last_string.substr(0,5) == "Drone"){
			int ind = last_string[5]-'1';
			if(first_string[0] == 'P'){
				Drones[ind].P = stoi(row[1]);
			}
			else if(first_string[0] == 'Q'){
				Drones[ind].Q = stoi(row[1]);
			}
			else if(first_string[0] == 'A'){
				Drones[ind].A = stod(row[1]);
			}
			else if(first_string[0] == 'B'){
				Drones[ind].B = stod(row[1]);
			}
			else if(first_string[0] == 'C'){
				Drones[ind].C = stod(row[1]);
			}
			else{
				Drones[ind].count = stoi(row[1]);
			}
		}
		else if(last_string.substr(0,12) == "WH Location "){
			int ind = first_string[2]-'1';
            if(first_string[3] == 'X'){
				Warehouses[ind].Position.x = stoi(row[1]);
			}
			if(first_string[3] == 'Y'){
				Warehouses[ind].Position.y = stoi(row[1]);
			}
			if(first_string[3] == 'Z'){
				Warehouses[ind].Position.z = stoi(row[1]);
			}
		}
	}
}

// Helper function to set timing from given string HH:MM:SS or H:MM:SS
void Set_Timing(Timing &inp, string s){
	int cnt = 0;
	for(auto i: s){
		if(i == ':')	cnt++;
		else if(cnt == 0)	inp.hr = inp.hr * 10 + (i - '0');
		else if(cnt == 1)	inp.min = inp.min * 10 + (i - '0');
		else if(cnt == 2)	inp.sec = inp.sec * 10 + (i - '0');
	}
	inp.convert_to_sec();
}

// Helper function to take input from the Demand File
void Take_Input_Demand(){
	vector<string> row;
	fstream file (demand_file, ios::in);
	string line, word;
	string first_string,last_string;
	int row_no=0;
	while(getline(file, line))
	{
		row_no++;

		row.clear();
		stringstream str(line);
		while(getline(str, word, ',')){
			row.push_back(word);
		}

		if(row_no == 1) continue;

		int sz = row[1].size();
		int ind = (stoi(row[1].substr(1, sz - 1)) - 1);
		int item_no = (*row[2].rbegin()-'1');
		int wh = (row[0].back()-'1');
		Demands[ind].demand_id = row[1];
		Demands[ind].Delievery_Point.x = stoi(row[4]);
		Demands[ind].Delievery_Point.y = stoi(row[5]);
		Demands[ind].Delievery_Point.z = stoi(row[6]);
		Demands[ind].Day = (row[3].back()-'1');
		Demands[ind].item_id = item_no;
		Demands[ind].warehouse_id = wh;
		Set_Timing(Demands[ind].Delievery_To,row[8]);
		Set_Timing(Demands[ind].Delievery_From,row[7]);
		Demands[ind].Delievery_Failure = (row[9][0] == '1');
		demand_count++;
	}
}
// Helper function to initialize all the drones to 8:00 (start of the day) and full battery consumption for the given DroneType
// In the beginning, we take that, drone belongs to no warehouse, so it can be used at whatever warehouse according to the strategy
void initialise_drone(){
	for(int i=0; i<Drone_Type_Count; i++){
        Drones[i].last_demand.resize(Drones[i].count);
		Drones[i].current_battery.resize(Drones[i].count);
        Drones[i].on_warehouse.resize(Drones[i].count);
        for(int j=0; j<Drones[i].count; j++){
			Drones[i].last_demand[j] = {8,0,0,8*3600};
			Drones[i].current_battery[j] = Drones[i].battery_capacity;
            Drones[i].on_warehouse[j] = -1;
		}
		Drones[i].status.resize(Drones[i].count);
	}
}

// Helper functor to sort the Demands array
bool comp(Demand a, Demand b){
	return a.Delievery_From.to_sec < b.Delievery_From.to_sec;
}

// Sort the Demands array on basis of Delivery_From
void sort_demand_by_time(){
	sort(Demands, Demands + demand_count, comp);
}

// Checks weather Drone can carry the requested demand (weight and volume)
bool is_compatible(Drone &drone, Demand &demand){
	if(drone.payload_capacity_weight < Items[demand.item_id].weight or drone.payload_capacity_volume < Items[demand.item_id].volume)
		return 0;
	return 1;
}

/*
	Finds the time taken and battery consumed by the drone to make the trip from/to (0,0,0) -> (x,y,z)
	Input: 
		P, Q, A, B, C, capacity, base_weight: Drone Parameters
		payload: payload carried by the Drone
		z_dir == 0 ? (0,0,0) -> (x,y,z) : (x,y,z) -> (0,0,0): Decides weather the trip is to/from 
	Output:
		pair<int, long double>: <Total Time Taken, Total battery consumed> for the trip
*/
pair<int, ld> find_time_battery(int x, int y, int z, int p, int q, int payload, int capacity, ld a, ld b, ld c, int base_weight, bool z_dir = 0){
	ld xy_speed, z_speed;
	ld f = (ld)payload / (ld)capacity;
	x = abs(x);
	y = abs(y);
	z = abs(z);
	if(z_dir == 0)
		z_speed = max_speed - q * f;
	else
		z_speed = max_speed + q * f;
	xy_speed = max_speed - p * f;
	int xy_time = ceil(sqrt(x * x + y * y) / xy_speed);
	int z_time = ceil(z / z_speed);
	int time_taken = xy_time + z_time;
	int w = base_weight + payload;
	ld first_term = (ld)w * a * time_taken;
	ld second_term = (ld)w * b * (xy_speed * xy_time + z_speed * z_time);
	ld third_term = z_dir == 0 ? (ld)w * c * z : 0;
	ld battery_consumed = first_term + second_term + third_term;
	return {time_taken, battery_consumed};
}		

/*
	Returns the cost if the current DroneType is used to satisfy the given Demand. If DroneType can satisfy the current Demands, it finds the best Drone of given Type that should be used, else -1
	Input:
		drone: DroneType to be used
		demand: Requested Demand
	Output:
		<long double, int>: {Total Battery Consumed, Best drone of current DroneType}
*/
pair<ld, int> find_battery(Drone &drone, Demand &demand){
	if(!is_compatible(drone, demand))
		return {INF, -1};
	pair<int, ld> tmp_to = find_time_battery(demand.Delievery_Point.x, demand.Delievery_Point.y, demand.Delievery_Point.z, drone.P, drone.Q, Items[demand.item_id].weight, drone.payload_capacity_weight, drone.A, drone.B, drone.C, drone.base_weight, 0);
	pair<int, ld> tmp_return = find_time_battery(demand.Delievery_Point.x, demand.Delievery_Point.y, demand.Delievery_Point.z, drone.P, drone.Q, 0, drone.payload_capacity_weight, drone.A, drone.B, drone.C, drone.base_weight, 1);
	int total_time = tmp_to.first + tmp_return.first;
	int total_battery_consumed = tmp_to.second + tmp_return.second;
	int best_drone = -1, min_start_time;
	ld cost = INF;
	for(int i=0; i<drone.count; i++){
        if(drone.on_warehouse[i] != -1 and drone.on_warehouse[i] != demand.warehouse_id)
            continue;
    	if((drone.last_demand[i].to_sec + tmp_to.first + 360) > demand.Delievery_To.to_sec)
			continue;
    	int battery_usage = total_battery_consumed - drone.current_battery[i];
		if(battery_usage > 0){
			int rem_time = demand.Delievery_To.to_sec - (drone.last_demand[i].to_sec + tmp_to.first + 360 + 4);
			if(rem_time * 5 < battery_usage)
				continue;
		}
    	int start_time = drone.last_demand[i].to_sec;
		start_time = max(start_time, demand.Delievery_From.to_sec - tmp_to.first - 180);
		if(battery_usage > 0)
			start_time = max(start_time, drone.last_demand[i].to_sec + ((battery_usage + 4) / 5) + 1);
		if(best_drone == -1 or start_time < min_start_time){
			start_time = min_start_time;
			best_drone = i;
			cost = total_battery_consumed;
		}
	}
    return {total_battery_consumed, best_drone};
}

/*
	Adds the given activity for the current drone to be outputted
	Input:
		demand: Demand the drone is working on
		time: Time at which the activity is occcured
		drone_type: drone_type currently being used
		drone_id: ID of particular DroneType being used
		x, y, z: coordinates of the drone
		payload: payload at given instant
		speed: speed at given instant
		relevant_id, act: provides in table below
		mah_consumed: battery consumed at given instant
	relevant_id		act		description
	warehouse_id	"C-WH"	Charging at WH. X denotes 1,2,3.
	rs_id			"C-RS"	Charging at Recharge Station
	warehouse_id	"R-WH"	Resting at WH
	rs_id			"R-RS"	Resting at Recharge Station
	-1				"D"		Waiting at demand point to deliver demand D[X]
	warehouse_id	"PU-WH"	Waiting at WH to pick-up item
	-1				"END"	END of the day
	-1				"T-L"	Traveling with payload
	-1				"T-E"	Traveling empty
*/
void Print_Activity(Demand &demand, int time, int drone_type, int drone_id, ld x, ld y, ld z, int payload, ld speed, int relevant_id, string act, ld mah_consumed = 0){
	Drone& drone = Drones[drone_type];
	time -= 8*3600-1;
	string drone_name = "D";
	drone_name += drone_type+'1';
	drone_name += char('A'+drone_id);
	string activity = act;
	if (relevant_id != -1 ) activity += '0'+relevant_id;
    x += Warehouses[demand.warehouse_id].Position.x;
    y += Warehouses[demand.warehouse_id].Position.y;
	vector<string> row = {drone_name, day, to_string(time), to_string(x), to_string(y), to_string(z), activity, to_string(payload), to_string(speed), "-", "-"};
	if (act == "T-L" or act == "T-E") {
		ld energy_cost = mah_consumed * Cost_Of_Charging;
		row[(int)row.size()-2] = to_string(mah_consumed);
		row[(int)row.size()-1] = to_string(energy_cost);
	}
	Drones[drone_type].status[drone_id].push_back(row);
}

/*
	Finds Instantaneous battery consumed to move from (x,y,z) -> (new_x, new_y, new_z) with give speed and payload
	Input:
		x, y, z: Coordinates at the beginning of the instant
		new_x, new_y, new_z: Coordinates at the end of the instant
		a, b, c, base_weight: Drone Paremeters
		speed: Speed of the drone at the given instant
		payload: Payload carried by the drone at the given instant
	Output:
		long double: Instantaneous battery consumed
*/
ld find_battery_consumed(ld x, ld y, ld z, ld new_x, ld new_y, ld new_z, ld a, ld b, ld c, ld speed, int payload, ld base_weight){
	ld w = payload + base_weight;
	ld first_term, second_term, third_term;
	first_term = w * a;
	second_term = w * b * speed;
	if(new_z > z)
		third_term = w * c * (new_z - z + 0.0);
	else
		third_term = 0;
	return first_term + second_term + third_term;
}

/*
	Gets the path to be used in the output of the DronePath file
	Input:
		drone_type: Type of Drone to be used
		drone_id: Id of the Drone for particular DroneType to be used
		demand: Demand to be satisfied
*/
void get_output(int drone_type, int drone_id, Demand &demand){
	Drone& drone = Drones[drone_type];
    drone.on_warehouse[drone_id] = demand.warehouse_id;
	pair<int, ld> tmp_to = find_time_battery(demand.Delievery_Point.x, demand.Delievery_Point.y, demand.Delievery_Point.z, drone.P, drone.Q, Items[demand.item_id].weight, drone.payload_capacity_weight, drone.A, drone.B, drone.C, drone.base_weight, 0);
	pair<int, ld> tmp_return = find_time_battery(demand.Delievery_Point.x, demand.Delievery_Point.y, demand.Delievery_Point.z, drone.P, drone.Q, 0, drone.payload_capacity_weight, drone.A, drone.B, drone.C, drone.base_weight, 1);
	int total_time = tmp_to.first + tmp_return.first;
	int total_battery_consumed = tmp_to.second + tmp_return.second;
	int battery_usage = total_battery_consumed - drone.current_battery[drone_id];
	int start_time = drone.last_demand[drone_id].to_sec;
	start_time = max(start_time, demand.Delievery_From.to_sec - tmp_to.first - 180 + 3);
	if(battery_usage > 0)
		start_time = max(start_time, drone.last_demand[drone_id].to_sec + ((battery_usage + 4) / 5) + 1);
	int& time = drone.last_demand[drone_id].to_sec;
	ld& battery = drone.current_battery[drone_id];
	ld x = 0, y = 0, z = 0;
    int payload = 0, speed = 0;
	while(time < start_time){
		if(battery < drone.battery_capacity){
			Print_Activity(demand, time, drone_type, drone_id, x, y, z, 0, 0, demand.warehouse_id+1, "C-WH");
			battery += 5;
			if(battery > drone.battery_capacity)
				battery = drone.battery_capacity;
		}
		else{
			Print_Activity(demand, time, drone_type, drone_id, x, y, z, 0, 0, demand.warehouse_id+1, "R-WH");
		}
		time++;
	}
	for(int i=0; i<180; i++){
		payload = Items[demand.item_id].weight;
		Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, 0, 1, "PU-WH");
		time++;
	}
	int p = drone.P, q = drone.Q, capacity = drone.payload_capacity_weight;
	ld xy_speed, z_speed, x_speed, y_speed;
	ld f = (ld)payload / (ld)capacity;
	z_speed = max_speed - q * f;
	xy_speed = max_speed - p * f;
	ld target_x = demand.Delievery_Point.x, target_y = demand.Delievery_Point.y, target_z = demand.Delievery_Point.z;
	x_speed = abs(xy_speed * (ld)target_x / (sqrt(target_x * target_x + target_y * target_y)));
    y_speed = abs(xy_speed * (ld)target_y / (sqrt(target_x * target_x + target_y * target_y)));
    ld a = drone.A, b = drone.B, c = drone.C; 
	// go xy
	ld battery_consumed;
	while(abs(x) < abs(target_x)){
        ld new_x = abs(x) + x_speed;
        ld new_y = abs(y) + y_speed;
		new_x = min(abs(target_x), new_x);
        new_y = min(abs(target_y), new_y);
		if(target_x < 0)	new_x = -new_x;
        if(target_y < 0)	new_y = -new_y;
		battery_consumed = find_battery_consumed(x, y, z, new_x, new_y, z, a, b, c, xy_speed, payload, drone.base_weight);
		x = new_x;
        y = new_y;
		battery -= battery_consumed;
		Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, xy_speed, -1, "T-L", battery_consumed);
		time++;
	}
	// go z
	while(abs(z) < abs(target_z)){
		ld new_z = abs(z) + z_speed;
		new_z = min(abs(target_z), new_z);
		if(z < 0)	new_z = -new_z;
		battery_consumed = find_battery_consumed(x, y, z, x, y, new_z, a, b, c, z_speed, payload, drone.base_weight);
		z = new_z;
		battery -= battery_consumed;
		Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, z_speed, -1, "T-L", battery_consumed);
		time++;
	}
	for(int i=0; i<180; i++){
		Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, 0, -1, demand.demand_id);
		time++;
	}
	if(failure == 0 or demand.Delievery_Failure == 0){
        payload = 0;
    }
    f = 0;
	z_speed = max_speed + q * f;
	xy_speed = max_speed - p * f;
	// return z
	while(abs(z) > 0){
		ld new_z = abs(z) - z_speed;
		new_z = max((ld)0, new_z);
		if(z < 0)	new_z = -new_z;
		battery_consumed = find_battery_consumed(x, y, z, x, y, new_z, a, b, c, z_speed, payload, drone.base_weight);
		z = new_z;
		battery -= battery_consumed;
		if(payload == 0)
            Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, z_speed, -1, "T-E", battery_consumed);
		else
            Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, z_speed, -1, "T-L", battery_consumed);
        time++;
	}
	// return xy
	while(abs(x) > 0){
		ld new_x = abs(x) - x_speed;
        ld new_y = abs(y) - y_speed;
		new_x = max((ld)0, new_x);
        new_y = max((ld)0, new_y);
		if(target_x < 0)	new_x = -new_x;
        if(target_y < 0)	new_y = -new_y;
		battery_consumed = find_battery_consumed(x, y, z, new_x, new_y, z, a, b, c, xy_speed, payload, drone.base_weight);
		x = new_x;
        y = new_y;
		battery -= battery_consumed;
		if(payload == 0)
            Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, xy_speed, -1, "T-E", battery_consumed);
		else
            Print_Activity(demand, time, drone_type, drone_id, x, y, z, payload, xy_speed, -1, "T-L", battery_consumed);
        time++;
	}
}

/*
	Finds best particular DroneType to be used to satisfy the given demand and returns minimum cost
	Input:
		demand: demand to be satisfied
	Output:
		long double: minimum cost to satisfy the demand if can be satisfied else INF
*/
ld find_best_available_drone(Demand &demand){
	int min_cost = INF;
	int best_drone_ind = -1, ind = 0, best_drone_id = -1;
	for(int i=0; i<Drone_Type_Count; i++){
		pair<ld, int> tmp = find_battery(Drones[i], demand);
		if(tmp.first != INF and tmp.first < min_cost and tmp.second != -1){
			min_cost = tmp.first;
			best_drone_ind = ind;
			best_drone_id = tmp.second;
		}
		ind++;
	}
    if(best_drone_ind != -1){
		get_output(best_drone_ind, best_drone_id, demand);
    }
    else{
        cout << demand.demand_id << endl;
        cout << demand.Delievery_From.to_sec << endl;
        cout << demand.Delievery_To.to_sec << endl;
        demand_failed++;
        exit(0);
    }
	return min_cost;
}

/*
	Prints the output to the DronePath.csv (output file)
*/
void fcfs_output(){
	fstream file (output_file, ios::out | std::fstream::app);
	for(int i=0; i<Drone_Type_Count; i++){
		for(int j=0; j<Drones[i].count; j++){
			if(Drones[i].status[j].size() != 0){
				int time = Drones[i].status[j].size();
				Drones[i].status[j].push_back(Drones[i].status[j].back());
				Drones[i].status[j].back()[2] = to_string(stoi(Drones[i].status[j].back()[2])+1);
				Drones[i].status[j].back()[6] = "END";
				Drones[i].status[j].back()[8] = "0";
				Drones[i].status[j].back()[9] = "-";
				Drones[i].status[j].back()[10] = "-";
				for(auto v: Drones[i].status[j]){	
					int sz = v.size();
					for(int k=0; k<sz; k++){
						file << v[k];
						if(k==sz-1)	file << endl;
						else		file << ",";
					}
				}
			}
		}
	}
}

/*
	FCFS solver (first come first serve)
	Satisfies each demand in the order of DeliveryFrom (One Drone satisfies One Demands and returns to Warehouse)
*/
void fcfs_solve(){
	sort_demand_by_time();
    demand_failed = 0;
    ld total_cost = 0;
    for(int i=0; i<demand_count; i++){	
        Demands[i].Delievery_Point.x -= Warehouses[Demands[i].warehouse_id].Position.x;
        Demands[i].Delievery_Point.y -= Warehouses[Demands[i].warehouse_id].Position.y;
    }
    initialise_drone();
    for(int i=0; i<demand_count; i++){
        int cost = find_best_available_drone(Demands[i]);
        if(cost != INF)
            total_cost += cost;
    }
    cout << demand_failed << endl;
    fcfs_output();
}

/*
	Input Arguments:

	[1] Path of Parameters File
	[2] Path of Demand File
	[3] Path of Output File
	[4] Day of Demand Day: Day 1/2/3
	[5] Delivery Failure = 0/1 [bool]
*/
int main(int argc, char *argv[]) {
	parameter_file = argv[1];
	demand_file = argv[2];
	output_file = argv[3];
	day = argv[4];
	failure = stoi(argv[5]);
	cout << parameter_file << " " << demand_file << " " << output_file << " " << day << " " << failure << endl;
	Initialise_Items();
	Initialise_Drone_Details();
	Take_Input_Parameters();
	Take_Input_Demand();
	fcfs_solve();
	return 0;
}