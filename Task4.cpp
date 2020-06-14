 // Interarrival time has exponential distn.
// uniform distn for ATM 1
// triangular distn for ATM 2
// Normal distn for ATM 3
#include <iostream>
#include <random>
#include <array>
#include <chrono>
#include <thread>

using namespace std ;

const int experiment_size = 10;  // number of experiments
double ATM_CT[3] = {0.0,0.0,0.0} ;
double ATM_time_used[3] = {0.0,0.0,0.0} ;
//_____________________________________________________________
double Generate_exponential ()
{
    // construct a trivial random generator engine from a time-based seed:
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator1(seed);
    exponential_distribution<double> exp_distribution(1.0);
    double number1 = exp_distribution(generator1);
    return number1 ;
}
double Generate_uniform ()
{

    default_random_engine generator2;
    uniform_real_distribution<double> uni_distribution(2,4);
    double number2 = uni_distribution(generator2);
    return number2 ;

}
piecewise_linear_distribution<double> triangular_distribution(double min, double mode, double max)
{
    array<double, 3> i{min, mode, max};
    array<double, 3> w{0, 1, 0};
    //int seed = chrono::system_clock::now().time_since_epoch().count();
    //default_random_engine generator1;
    //default_random_engine generator;
    return piecewise_linear_distribution<double> {i.begin(), i.end(), w.begin()};
}
double Generate_triangular()
{
    random_device rd;
    // create a mersenne twister PRNG seeded from some implementation-defined random source
    mt19937 gen(rd());

    auto dist = triangular_distribution(2, 3.3, 4);
    double number3  =  dist(gen) ;
    return number3 ;
}
double Generate_normal()
{
    default_random_engine generator4;
    normal_distribution<double> norm_distribution(3.0,0.5);
    double number4 = norm_distribution(generator4);
    return number4 ;
}

int GOTO_ATM_Number ()
{
    double Available_soon = min(ATM_CT[0], min(ATM_CT[1], ATM_CT[2])) ;
    if (Available_soon == ATM_CT[ 0 ] || (ATM_CT[0] == ATM_CT[1] && ATM_CT[0] == ATM_CT[2] ))
        return  0 ; // we're somehow biased to ATM 1 ;
    else if (Available_soon == ATM_CT[ 1 ])
        return  1 ;
    else if (Available_soon == ATM_CT[ 2 ])
        return  2 ;
}
/*
double Appropriate_ATM ()
{
    double Available_soon = min(ATM_CT[0], min(ATM_CT[1], ATM_CT[2])) ;
    if (Available_soon == ATM_CT[ 0 ] || (ATM_CT[0] == ATM_CT[1] && ATM_CT[0] == ATM_CT[2] ))
        return  ATM_CT[0] ; // we're somehow biased to ATM 1 ;
    else if (Available_soon == ATM_CT[ 1 ])
        return  ATM_CT[1] ;
    else if (Available_soon == ATM_CT[ 2 ])
        return  ATM_CT[2] ;
}
*/

int main()
{
    double system_time = 0 ;
    double Util_1 = 0, Util_2 = 0, Util_3 = 0 ;
    double total_waiting_time  = 0, Max_wait_time = 0 , max_CT = 0;
    double had_to_wait  = 0, wait_more_minute = 0;
    double pre_AT = 0 ; // arriavl time of previous customer
    double IAT, AT, SST, ST, WT, CT = 0 ;      // interarrival time ,arrival time , start time , service time , waiting time  , completion time
    double pre_CT = 0 ; // completion time of previous customer
    for (int i = 0 ; i < experiment_size ; i++)
    {
        IAT = Generate_exponential() ;
        AT = IAT + pre_AT ; // AT[i]  = AT [ i-1 ] + IAT [i]
        pre_AT = AT ;

        int ATM_number = GOTO_ATM_Number() ;
        if ( ATM_CT [ATM_number] <= AT ) // client came and found one available ATM
        {
            SST = AT ;
            WT = 0 ;
        }
        else            // ALL ATMs are attended
        {
            SST =  ATM_CT[ATM_number] ;    // start to use the next available ATM
            WT = SST - AT ;
            if (WT > Max_wait_time )
                Max_wait_time = WT ;
            total_waiting_time += WT ;
            had_to_wait++ ;
            if (WT > 1.0)
                wait_more_minute ++ ;
        }
        //__________________________________

        if (ATM_number == 0)
        {
            ST = Generate_uniform() ;
        }
        else if (ATM_number == 1)
        {
            ST = Generate_triangular() ;
        }
        else if (ATM_number == 2)
        {
            ST = Generate_normal() ;
        }
        CT = SST + ST ;  // completion time = start time + service time
        if (CT > max_CT) max_CT = CT ;
        ATM_time_used[ATM_number] += ST ;
        ATM_CT[ATM_number] = CT ;
//        cout << "Cust " << i+1 << " came after " << IAT  << " .. so he Arrives " << AT << " .. and waited " << WT << " and started " << SST << "\n\t"
//             << " .. service : " << ST << " .. and left at : "<< CT <<
//             endl << "\t .. and used ATM : " << ATM_number+1 <<  "//////////////////\n" << endl;

        if (i == experiment_size -1)
            system_time = max_CT ; // system time = CT of last client
    }
    double avg_WT = total_waiting_time / experiment_size ; // average waiting time
    double Prob_of_waiting = had_to_wait / experiment_size ;
    double Prob_of_waiting_more_minute = wait_more_minute / experiment_size ; // prob. of waiting more than 1 minute
//    double Prob_of_waiting_more_minute = wait_more_minute / had_to_wait ; // alternative way but not sure

    Util_1 = (ATM_time_used[0] / system_time ) * 100 ;
    Util_2 = (ATM_time_used[1] / system_time  ) * 100;
    Util_3 = (ATM_time_used[2] / system_time ) * 100 ;

    cout << "average waiting time : " << avg_WT << endl ;
    cout << "Maximum waiting time : " << Max_wait_time << endl ;
    cout << "probability of waiting : " << Prob_of_waiting << endl ;
    cout << "probability of waiting more than 1 minute : " << Prob_of_waiting_more_minute << endl ;

    cout << "Utilization of ATM 1 : " << Util_1 << " % " << endl;
    cout << "Utilization of ATM 2 : " << Util_2 << " % " << endl;
    cout << "Utilization of ATM 3 : " << Util_3 << " % " << endl;




    return 0;
}
