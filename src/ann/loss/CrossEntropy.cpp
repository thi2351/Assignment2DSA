/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   CrossEntropy.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"
#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction): ILossLayer(reduction){
    
}

CrossEntropy::CrossEntropy(const CrossEntropy& orig):
ILossLayer(orig){
}

CrossEntropy::~CrossEntropy() {
}

double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t) {
    m_aCached_Ypred = X;
    m_aYtarget = t;
    
    xt::xarray<double> log_likelihood = xt::log(X + 1e-9); // Thêm epsilon để tránh log(0)
    xt::xarray<double> cross_entropy = -xt::sum(t * log_likelihood, {1}); // Tính trên từng mẫu

    // Áp dụng reduction
    if (this->m_eReduction == REDUCE_MEAN) {
        return xt::mean(cross_entropy)();
    } else if (this->m_eReduction == REDUCE_SUM) {
        return xt::sum(cross_entropy)();
    } else {
        return cross_entropy();
    }
}

xt::xarray<double> CrossEntropy::backward() {
    const double EPSILON = 1e-9; 
    int N_norm = m_aCached_Ypred.shape()[0]; 

    xt::xarray<double> grad = - (m_aYtarget / (m_aCached_Ypred + EPSILON)) / N_norm;

    return grad;
}