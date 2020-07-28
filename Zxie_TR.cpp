
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <omp.h> 
#include <chrono>
#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <bitset>
#include <graphblas/graphblas.hpp>
#include "Timer.hpp"
using namespace grb;
using namespace std;
using namespace std::chrono;
#define MAX(a, b) ((a) > (b) ? (a) : (b) )
#define MIN(a, b) ((a) < (b) ? (a) : (b) )
#define White 0
#define Gray 1
#define Black 2
#define INF (1<<31)-1
//****************************************************************************
IndexType read_edge_list(std::string const &pathname,
                         IndexArrayType &row_indices,
                         IndexArrayType &col_indices)
{
    std::ifstream infile(pathname);
    IndexType max_id = 0;
    uint64_t num_rows = 0;
    uint64_t src, dst;

    while (infile)
    {
        infile >> src >> dst;
        // std::cout << "Read: " << src << ", " << dst << std::endl;
        max_id = std::max(max_id, src);
        max_id = std::max(max_id, dst);

        //if (src > max_id) max_id = src;
        //if (dst > max_id) max_id = dst;

        row_indices.push_back(src);
        col_indices.push_back(dst);

        ++num_rows;
    }

    std::cout << "Read " << num_rows << " rows." << std::endl;
    std::cout << "#Nodes = " << (max_id + 1) << std::endl;

    //////////////////////must have nodea->nodea!!!!!!!!!!!!!!!!!
    int temp = row_indices.size();
    for(int i = 0; i<max_id + 1;i++){
        row_indices.push_back(i);
        col_indices.push_back(i);
    }
    return (max_id + 1);
}

//****************************************************************************
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "ERROR: too few arguments." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <edge list file>" << std::endl;
        exit(1);
    }

    // Read the edgelist and create the tuple arrays
    std::string pathname(argv[1]);
    IndexArrayType iA, jA;

    IndexType const NUM_NODES(read_edge_list(pathname, iA, jA));


    using T = int32_t;
    using MatType = Matrix<T>;
    using BoolMatType = Matrix<bool>;
    std::vector<T> v(iA.size(), 1);
    std::vector<bool> bv(iA.size(), true);
    MatType A(NUM_NODES, NUM_NODES);
    MatType B(NUM_NODES, NUM_NODES);
    BoolMatType M(NUM_NODES, NUM_NODES);

    A.build(iA.begin(), jA.begin(), v.begin(), iA.size());
    B.build(iA.begin(), jA.begin(), v.begin(), iA.size());
    M.build(iA.begin(), jA.begin(), bv.begin(), iA.size());
    // print_matrix(std::cout, A, "Matrix A");
    // print_matrix(std::cout, B, "Matrix B");
    std::cout << "Running algorithm(s)... nvals = " << A.nvals() << std::endl;
    std::cout <<"NUM_NODES = "<<NUM_NODES<<std::endl;
    Timer<std::chrono::steady_clock, std::chrono::microseconds> my_timer;
    MatType C(NUM_NODES, NUM_NODES);

    my_timer.start();

    mxm(C, NoMask(), NoAccumulate(), 
        ArithmeticSemiring<double>(), 
        A, B);
    // print_matrix(std::cout, C, "Matrix C1");

    mxm(C, NoMask(), NoAccumulate(), 
        ArithmeticSemiring<double>(), 
        C,C);
    // print_matrix(std::cout, C, "Matrix C2");


    //===================
    // A*B
    //===================
    std::cout << "OPTIMIZED IMPLEMENTATION: F/W-----------" << std::endl;

    for(int i =0; i<NUM_NODES;i++){
        int temp =C.nvals();
        
        mxm(C, NoMask(), NoAccumulate(),
            ArithmeticSemiring<double>(),
            C, C);

        // print_matrix(std::cout, C, "Matrix C");


        if(temp==C.nvals()){
            std::cout << "----break\n ";
            break;
        }

    }

        my_timer.stop();
        std::cout << "time use(ms):         : " << my_timer.elapsed()
                << " usec, C.nvals = " << C.nvals()-NUM_NODES << std::endl;

     std::cout << "-----------------------------------===\n ";


     return 0;
}