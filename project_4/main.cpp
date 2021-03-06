#include <iostream>
#include "lattice.hpp"

void loop_over_temp(int L, std::string filename, int cycles)
{
    int N = L*L;
    // loop over temperature
    // define my variables
    arma::vec eps_list(cycles);
    arma::vec m_list(cycles);
    
    auto t1 = std::chrono::steady_clock::now();
    double T0 = 2.1; double T1 = 2.4;
    int n = 100;
    // define my vectors
    arma::vec Temp = arma::linspace(T0, T1, n);
    arma::vec vec_Cv(n);
    arma::vec vec_chi(n);
    arma::vec vec_eps(n);
    arma::vec vec_m(n);
    // loop and fill vectors
    #pragma omp parallel for 
    for (int i=0; i<n; i++)
    {
        double Tempi = Temp(i);
        Lattice myinstance(L, Tempi, false);
        arma::vec average = myinstance.full_cycle(cycles, eps_list, m_list);
        double Cv = myinstance.specific_heat_capacity(average);
        double chi = myinstance.susceptibility(average);
        double eps = myinstance.energy_per_spin_expectation(average);
        double m = myinstance.magnetization_per_spin_expectation(average);
        vec_Cv(i) = Cv;
        vec_chi(i) = chi;
        vec_eps(i) = eps;
        vec_m(i) = m;
        std::cout << i <<"/100"<< std::endl; 
    }

    auto t2 = std::chrono::steady_clock::now();
    std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()<<" milliseconds"<<"\n";
    Temp.save("Temp"+filename);
    vec_Cv.save("Cv"+filename);
    vec_chi.save("chi"+filename);
    vec_eps.save("expect_eps"+filename);
    vec_m.save("expect_m"+filename);
}

int main()
{
    // INITIALIZE PROGRAM
    std::cout << "Welcome to our program! Here are your options:" << '\n';
    std::cout << "  [1] Run simulation for L=2, 10000 cycles" << '\n';
    std::cout << "  [2] Run simulation for L=20, 250000 cycles" << '\n';
    std::cout << "  [3] Run simulation for L=40, 250000 cycles" << '\n';
    std::cout << "  [4] Run simulation for L=60, 250000 cycles" << '\n';
    std::cout << "  [5] Run simulation for L=80, 250000 cycles" << '\n';
    std::cout << "  [6] Run simulation for L=100, 250000 cycles" << '\n';
    std::cout << "  [7] Run simulation for L=n and cycles=m" << '\n';
    std::cout << "  [8] Burn in test for L=20, unordered/ordered, T=1 & T=2.4" << '\n';
    std::cout << "  [9] Approximate eps for L=20, T=1 and T=2.4 \n";

    
    std::cout << '\n' << "Please select an option" << "\n";
    int x;
    std::cin >> x;
    switch(x) 
    {
        // loop over temperature
        // define my variables
        case 1: 
        {
            int L = 2;
            int N = L*L;
            double t1 = clock();
            double T0 = -1; double T1 = 1;
            int n = 100;
            double cycles = 10000;
            arma::vec eps_list(cycles);
            arma::vec m_list(cycles);
            // define my vectors
            //arma::vec Temp = arma::linspace(T0, T1, n);
            arma::vec Temp = arma::logspace(T0, T1, n);
            arma::vec vec_Cv(n);
            arma::vec vec_chi(n);
            arma::vec vec_eps(n);
            arma::vec vec_m(n);
            // loop and fill vectors
            #pragma omp parallel for
            for (int i=0; i<n; i++)
            {
                double Tempi = Temp(i);
                Lattice myinstance(L, Tempi, false);
                arma::vec average = myinstance.full_cycle(cycles, eps_list, m_list);
                double Cv = myinstance.specific_heat_capacity(average);
                double chi = myinstance.susceptibility(average);
                double eps = myinstance.energy_per_spin_expectation(average);
                double m = myinstance.magnetization_per_spin_expectation(average);
                vec_Cv(i) = Cv;
                vec_chi(i) = chi;
                vec_eps(i) = eps;
                vec_m(i) = m;
                std::cout << i << "/" << n << '\n';
            }

            //std::cout << vec_Cv << '\n';
            //std::cout << vec_chi << '\n';
            double t2 = clock();
            double duration_seconds = ((double) (t2-t1))/CLOCKS_PER_SEC;
            std::cout << "Time: " << duration_seconds << '\n';
            Temp.save("Temp10.txt");
            vec_Cv.save("Cv10.txt");
            vec_chi.save("chi10.txt");
            vec_eps.save("eps10.txt");
            vec_m.save("m10.txt");

            break;
        }
        
        case 2:
        {
            loop_over_temp(20, "_L20.txt", 250000);
            break;
        }

        case 3: 
        {
            loop_over_temp(40, "_L40.txt", 250000);
            break;
        }

        case 4:
        {
            loop_over_temp(60, "_L60.txt", 250000);
            break;
        }

        case 5:
        {
            loop_over_temp(80, "_L80.txt", 250000);
            break;
        }

        case 6:
        {
            loop_over_temp(100, "_L100.txt", 250000);
            break;
        }

        case 7:
        {   
            int L;
            int cycles;
            std::cout << "Input lattice size: ";
            std::cin >> L;
            std::cout << "Input number of Monte Carlo cycles: ";
            std::cin >> cycles;
            loop_over_temp(L, "_Ln.txt", cycles);
            break;
        }

        case 8:
        {   
            int cycles = 100000;
            
            auto t1 = std::chrono::steady_clock::now();
            arma::vec eps_list(cycles);
            arma::vec m_list(cycles);
            Lattice myinstance(20, 1, true);
            myinstance.full_cycle(cycles, eps_list, m_list);
            eps_list.save("eps_burn_in_test_ordered_T1.txt");
            m_list.save("m_burn_in_test_ordered_T1.txt");
            
            auto t2 = std::chrono::steady_clock::now();
            std::cout<<"Calculations done for ordered T=1, cycles=" << cycles << std::endl;
            std::cout<<"Time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()<<" milliseconds"<<"\n";

            auto t3 = std::chrono::steady_clock::now();
            arma::vec eps_list2(cycles);
            arma::vec m_list2(cycles);
            Lattice myinstance2(20, 1, false);
            myinstance2.full_cycle(cycles, eps_list2, m_list2);
            eps_list2.save("eps_burn_in_test_unordered_T1.txt");
            m_list2.save("m_burn_in_test_unordered_T1.txt");

            auto t4 = std::chrono::steady_clock::now();
            std::cout<<"Calculations done for unordered T=1, cycles=" << cycles << std::endl;
            std::cout<<"Time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count()<<" milliseconds"<<"\n";

            auto t5 = std::chrono::steady_clock::now();
            arma::vec eps_list3(cycles);
            arma::vec m_list3(cycles);
            Lattice myinstance3(20, 2.4, true);
            myinstance3.full_cycle(cycles, eps_list3, m_list3);
            eps_list3.save("eps_burn_in_test_ordered_T24.txt");
            m_list3.save("m_burn_in_test_ordered_T24.txt");

            auto t6 = std::chrono::steady_clock::now();
            std::cout<<"Calculations done for ordered T=2.4, cycles=" << cycles << std::endl;
            std::cout<<"Time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(t6-t5).count()<<" milliseconds"<<"\n";

            auto t7 = std::chrono::steady_clock::now();
            arma::vec eps_list4(cycles);
            arma::vec m_list4(cycles);
            Lattice myinstance4(20, 2.4, false);
            myinstance4.full_cycle(cycles, eps_list4, m_list4);
            eps_list4.save("eps_burn_in_test_unordered_T24.txt");
            m_list4.save("m_burn_in_test_unordered_T24.txt");

            auto t8 = std::chrono::steady_clock::now();
            std::cout<<"Calculations done for unordered T=2.4, cycles=" << cycles << std::endl;
            std::cout<<"Time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(t8-t7).count()<<" milliseconds"<<"\n";
            
        }

        case 9:
        {
            int cycles = 250000;
            Lattice myinstance(20, 1, false);
            arma::vec eps_list(cycles);
            arma::vec m_list(cycles);
            myinstance.full_cycle(cycles, eps_list, m_list, true);
            eps_list.save("approximate_eps_T1.txt");

            Lattice myinstance2(20, 2.4, false);
            arma::vec eps_list2(cycles);
            myinstance2.full_cycle(cycles, eps_list2, m_list, true);
            eps_list2.save("approximate_eps_T24.txt");
        }
        
    }


    return 0;
}



