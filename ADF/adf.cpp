#include "adf.h"

#include <armadillo>
#include <iostream>

adf::adf(OPTIONS option):
    option(option),
    y(arma::vec()),
    design_adf(arma::mat()),
    beta(arma::vec()),
    regression(arma::mat(), arma::vec()),
    k(0)
{ }

adf::adf(OPTIONS option, arma::vec y):
    option(option),
    y(y),
    design_adf(arma::mat()),
    beta(arma::vec()),
    regression(arma::vec(), y),
    k(0)
{ }

adf::adf(OPTIONS, arma::vec y, int k):
    option(option),
    y(y),
    design_adf(arma::mat()),
    beta(arma::vec()),
    regression(design_adf, y),
    k(k)
{ }

void adf::getDesign_adf() const
{
    design_adf.print("design:");
}

void adf::getObservation() const
{ 
    y.print("observation:");
}

void adf::getBeta() const
{ 
    beta.print("beta:");
}

void adf::getStatistics() const
{
   std::cout << "The unit root t-test statistics is " << statistics << std::endl;
}

void adf::setDesign(arma::mat d)
{ 
    design_adf = d;
    regression.setDesign(d);
}

void adf::setObservation(arma::vec obs)
{ 
    y = obs;
    regression.setObservation(obs);
}

void adf::evaluateSE(int k)
{       
    /*int iter;
    int i = 0;
    arma::vec store = arma::vec(y.n_elem-4);
    for (iter = 4; iter< y.n_elem; ++iter){
        store(iter-4) = evaluatePhi(k, iter);
    } //not computing phi for the first 4 observations
    
    phi = store(store.n_elem-1);  //access final element
    std::cout << "The calculated value of phi is " << phi << std::endl;
    */

    phi = evaluatePhi(k);
    se_phi = regression.evaluateBetaSE();

    statistics = phi/se_phi;

}                

double adf::evaluatePhi(int k) 
{ 
    arma::vec x = y;

    arma::vec y_ = arma::vec(x.n_elem+1); //y_ is the y_{t-1} of y
    
    y_(0) = 0;
    
    for (int i = 1; i<x.n_elem+1; i++){
        y_(i) = x(i-1);
    }
    
    

    switch (option) { 
        {case OPTIONS::DF:
            arma::vec product = x%y_;      
            
            arma::vec denom = x%x;
            phi = sum(product)/sum(denom);
                      
            return phi;
            break;
        }    
        
        {case OPTIONS::ADF:
            arma::mat fix = arma::mat(x.n_elem+1, 3); //n+1*3 arma matrix
            arma::mat lag = arma::mat(x.n_elem+1, k);; //n+1*k arma matrix, where k is the number of lag terms in consideration 
            
            for (int i = 1; i<x.n_elem+1; i++){
                fix(i,0) = 1;
                fix(i,1) = y_(i-1);
                fix(i,2) = i;
            }
           
           //STABLE VERSION fix = [x,x,x; 1,y_0,1; ... ; 1,y_n-1,n-1]
                      
            for (int i = 0; i < x.n_elem + 1; i++){                //loop through x.n_elem+1 # of rows
                for(int count = 0; count < k; count++){          //loop through k # fo columns
                    if (i <= count + 1)                           //if the time elapsed i is smaller than lagtime
                        lag(i,count) = 0;
                    else
                        lag(i,count) = y_(i-count-1) - y_(i-count-2);
                } 
            }

            //lag NOT STABLE FOR NOW
    
            lag.insert_cols(0, fix);           
            lag.shed_rows(0,k+1); //yolo             
            
            design_adf = lag;           
           
            design_adf.print("design_adf:");
 
            arma::vec x_diff = arma::vec(x.n_elem); 
            for (int i = 0; i < x.n_elem; i++){
                x_diff(i) = y_(i+1) - y_(i);
            }

            x_diff.shed_rows(0,k); //yolo 

            x_diff.print("x_diff:");

            regression.setDesign(design_adf);
            regression.setObservation(x_diff);
           
            regression.evaluate(); 

            beta = regression.getBeta();
 
            return beta(1);            
            break;
        }
    }            
}

void adf::loadCSV(const std::string& filename)
{ 
    arma::mat A = arma::mat();
    bool status = A.load(filename);

    if(status == true)
    {
        std::cout << "successfully loaded" << std::endl;
    }

    else
    {
        std::cout << "problem with loading" << std::endl;
    }
    
    y = A;
    regression.setObservation(y);
}

void adf::saveBetaCSV()
{ 
    beta.save("beta.mat", arma::csv_ascii);
}
