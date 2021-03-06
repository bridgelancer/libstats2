//The class adf constructs the dickey-fuller and augmented dickey-fuller (ADF) tests as two distinct
//methods.

#include <armadillo>

#include "../OLS/ols.h"

enum class OPTIONS{
    DF,
    ADF
};

class adf{
public:
    adf(OPTIONS option);
    adf(OPTIONS option, arma::vec y);
    adf(OPTIONS option, arma::vec y, int k);
    
    void getDesign_adf() const;
    void getObservation() const;
    void getBeta() const;
    void getStatistics() const;
    
    void setDesign(arma::mat design);
    void setObservation(arma::vec observation);

    double evaluatePhi(int k);
    void evaluateSE(int k);
 
    void loadCSV(const std::string& filename);
    void saveBetaCSV();

    double phi;
    ols regression; //temporary public; for testing purposes only****
    arma::vec y; //The time-series data, notably the stock price
private:
    double se_phi;
  
    double statistics;
 
    OPTIONS option; 
    int k;
    //ols regression; //call the ols(arma::mat design_adf, arma::vec y) constructor   

    arma::mat design_adf;
    arma::vec beta;    
};
