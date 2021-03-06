#include "functions.hpp"

int main()
{
    double N = 6;
    double h = 1/(N+1);
    double a = -1/(h*h);
    double d =  2/(h*h);

    arma::mat tri_matrix = tridiagonal_matrix(a, d, N);
    
    std::cout << "-----Tridiagonal Matrix------ \n";
    std::cout << tri_matrix << '\n';
    std::cout << "-----Armadillo------ \n";
    arma::vec eigval = arma::eig_sym(tri_matrix);
    arma::mat eigvec;
    
    arma::eig_sym(eigval, eigvec, tri_matrix);
    
    std::cout << eigval << '\n';
    std::cout << eigvec << '\n';
    
    arma::vec eigenval = eigenvalues(a, d, N);
    arma::mat eigenvec = eigenvectors(N);
    arma::mat normalised = arma::normalise(eigenvec);
    std::cout << "-----Analytical----- \n";
    std::cout << eigenval << "\n";
    std::cout << normalised << "\n";
    

    std::cout << "-----Max Element----- \n";
    arma::mat test_matrix = {{1, 0, 0, 0.5}, {0, 1, -0.7, 0}, {0, -0.7, 1, 0}, {0.5, 0, 0, 1}};
    arma::mat test_matrix_2 = {{1, sqrt(2), 2}, {sqrt(2), 3, sqrt(2)}, {2, sqrt(2), 1}};
    int k;
    int l;
    double max_element = largest_off_element(test_matrix, k, l, 4);
    std::cout << "Largest off element: " << max_element << "\n";
    std::cout << "Index of largest off element: " << k << " " << l << "\n";

    std::cout << "-----Rotation----- \n";
    int counter = 0;

    arma::mat R;
    R.eye(N,N);
    
    // Generate random N*N matrix
    arma::mat A = arma::mat(N, N).randn();  

    // Symmetrize the matrix by reflecting the upper triangle to lower triangle
    A = arma::symmatu(A);  

    std::cout << R << "\n";
    arma::vec V(N);
    int maxiter = N*N*N;
    bool converged = false;
    std::cout << tri_matrix<< "\n";
    jacobi_eigensolver(tri_matrix, V, R, maxiter, counter, converged, N);
    std::cout << tri_matrix<< "\n";
    std::cout << R << "\n";
    std::cout << V << "\n";
    std::cout << "Iterations : " << counter << "\n";

    std::cout << "------PROBLEM 7------ \n";
    double n = 100;
    double h_ = 1/(n+1);
    double a_ = -1/(h_*h_);
    double d_ =  2/(h_*h_);
    arma::vec x = arma::linspace(0,1,n);
    
    arma::mat v = tridiagonal_matrix(a_, d_, n);
    double last = n*n*n;
    int c = 0;
    bool conv = false;
    arma::mat R2;
    R2.eye(n,n);

    std::cout << R2 << "\n";
    arma::vec V2(n);
    jacobi_eigensolver(v, V2, R2, last, c, conv, n);

    std::cout << v << "\n";
    std::cout << R2 << "\n";
    std::cout << V2 << "\n";
    std::cout << counter << "\n";

    write_eig_to_file(V2, R2, n, "N_100");
    write_lin_to_file(x, n, "linspace_N_100");


    return 0;
}


//g++ main.cpp functions.cpp -o test.out -O2 -larmadillo -llapack -lblas



