#include "particle.hpp"

Particle::Particle(int M_, double h_, double dt_, double T_, 
    double xc_, double yc_, double sigmax_, double sigmay_, double px_, double py_)
{
    /*
    CONSTRUCTOR
    Creates an instance of the particle class.
    Args: 
        - M_        (int)           :   size of the full system
        - h_        (double)        :
        - dt_       (double)        :   size of the timestep
        - T_        (double)        :   value of the final time
        - xc_       (double)        :   center x coordinate of perturbation
        - yc_       (double)        :   center y coordinate of perturbation
        - sigmax_   (double)        :   width of perturbation in x
        - sigmy_    (double)        :   width of perturbation in y
        - px_       (double)        :   momenta of wave packet in x
        - py_       (double)        :   momenta of wave packet in y
    */
    //
    M = M_;
    h = h_;
    dt = dt_;
    T = T_; 
    xc = xc_;
    yc = yc_;
    sigmax = sigmax_; 
    sigmay = sigmay_;
    px = px_;
    py = py_;

    // construct potential based on input (SLIT/TUNNELING)
    // v0 = v0_;
    V = arma::cx_mat(M-2, M-2);
    V.ones();

    std::tie(A, B) = (*this).construct_AB();
    u = arma::cx_vec((M-2)*(M-2));
    std::cout << u << '\n';
    (*this).initial_state();
    std::cout << u << '\n';
}

void Particle::initial_state()
{
    /*
    Constructs the initial state of the wavefunction matrix using a gaussian
    perutbation with y center yc and y width defined by sigmay and x center
    at xc and x width defined by sigmax. px and py is the wave packet momenta.
    */
    
    arma::vec x = arma::linspace(0,1,M-2);
    arma::vec y = arma::linspace(0,1,M-2);
    double base_x = 2.0*sigmax*sigmax;
    double base_y = 2.0*sigmay*sigmay;
    const std::complex<double> i_imag(0, 1);

    std::complex<double> element;
    std::complex<double> sum = 0;


    for (int i=1; i<M-1; i++)
    {
        for (int j=1; j<M-1; j++)
        {
            double xxc = x(i-1) - xc;
            double yyc = y(j-1) - yc;
            element =  std::exp(-std::pow(xxc, 2)/base_x - std::pow(yyc,2)/base_y + i_imag*px*(xxc) + i_imag*py*(yyc));
            sum += std::conj(element)*element;
            u(transform_index(i,j)) = element;
            // std::cout << i << ' ' << j << '\n'; 
        }
    }
    // Normalize u
    std::cout << sum << '\n';
    std::cout << u << '\n';
    for (int k = 0; k<(M-2)*(M-2); k++)
        u(k) = u(k)/std::sqrt(sum);

    sum = 0;
    for (int k = 0; k<(M-2)*(M-2); k++)
        sum += std::conj(u(k))*u(k);

    std::cout << sum << '\n';
}


int Particle::transform_index(int i, int j)
{
    /*
    Transforms indexes int i, int j of 2D array to a 1D array.
    i, j must be 0 < i,j < M-1.
    Args: 
        i (int)             : index along first axis
        j (int)             : index along second axis
    Returns: 
        i + (M - 2)*j (int) : index in linear space given i and j.
    */
   return (i-1) + (M - 2)*(j-1);
}

std::tuple<arma::cx_mat, arma::cx_mat> Particle::construct_AB()
{
    /*
    Constructs A and B complex matrices used in updating the matrix.
    Returns:
        - std::make_tuple(A, B) (tuple<cx_mat, cx_mat>) : Tuple containing complex matrix A
        and B.
    */
    
    int n = M - 2;
    int N = std::pow(n, 2);

    // define constants. 
    std::complex<double> r = 1i*dt/(2*h*h);
    std::complex<double> constant1 = 1. + 4.*r;
    std::complex<double> constant2 = 1. - 4.*r;
    std::complex<double> constant3 = 1i*dt/2.;
    
    

    arma::cx_vec a(N);
    arma::cx_vec b(N);

    std::complex<double> z1, z2;

    for (int i = 1; i < n+1; i++)
    {
        for (int j = 1; j < n+1; j++)
        {
            z1 = constant1 + constant3*V(i-1,j-1);
            z2 = constant2 - constant3*V(i-1,j-1);

            a(transform_index(i, j)) = z1;
            b(transform_index(i, j)) = z2;

        }
    }

    arma::cx_mat A(N,N);
    arma::cx_mat B(N,N);


    // Handle the diagonal
    for (int i = 0; i < N; i++)
    {
        A(i,i) = a(i);

        B(i,i) = b(i);
    }
    // Next to diagonal
    A(1,0) = -r;
    A(0,1) = -r;

    B(1,0) =  r;
    B(0,1) =  r;
    for (int i = 0; i < N-1; i++)
    {
        if ((i+1)%n != 0)
        {
            A(i+1,i)    = -r;
            A(i,i+1)    = -r;

            B(i+1,i)    =  r;
            B(i,i+1)    =  r;
        }

    }
    // remaining elements
    for (int i = 0; i < N - n; i++)
    {
        A(i+n,i) = -r;
        A(i,i+n) = -r;

        B(i+n,i) =  r; 
        B(i,i+n) =  r;
    }

    return std::make_tuple(A, B);
}

void Particle::update_system()
{
    /*
    Updates the wavefunction matrix by one step.
    */
    arma::cx_vec b = B * u;
    u = arma::solve(A, b);
}

void Particle::potential(int slits)
{  
    switch(slits)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            break;
        }
        case 2:
        {
            double thickx = 0.02;
            double posxy = 0.5;
            double wally = 0.05;
            double openy = 0.05;
            double v0 = 10e+5;
            double cols_thick = std::ceil(thickx*(M-2));
            for(int k = 0; k < M-2; k++)
                {
                    for(int h = 0; h < cols_thick; h++);
                        V(k, (M-2)*posxy-h) = v0;
                        V(k, (M-2)*posxy+h) = v0;
                }
            

            std::cout << V << std::endl;
            break;
        }
        case 3:
        {
            break;
        }
    }
}
