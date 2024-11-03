/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   AdamParamGroup.cpp
 * Author: ltsach
 * 
 * Created on October 8, 2024, 1:43 PM
 */

#include "optim/AdamParamGroup.h"

AdamParamGroup::AdamParamGroup(double beta1, double beta2):
    m_beta1(beta1), m_beta2(beta2){
    //Create some maps:
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pFirstMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pSecondMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    //
    m_step_idx = 1;
    m_beta1_t = m_beta1;
    m_beta2_t = m_beta2;
}

AdamParamGroup::AdamParamGroup(const AdamParamGroup& orig):
    m_beta1(orig.m_beta1), m_beta2(orig.m_beta2){
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pFirstMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pSecondMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    //copy:
    *m_pParams = *orig.m_pParams;
    *m_pGrads = *orig.m_pGrads;
    *m_pFirstMomment = *orig.m_pFirstMomment;
    *m_pSecondMomment = *orig.m_pSecondMomment;
    //
    m_step_idx = 1;
    m_beta1_t = m_beta1;
    m_beta2_t = m_beta2;
}

AdamParamGroup::~AdamParamGroup() {
    if(m_pFirstMomment != nullptr) delete m_pFirstMomment;
    if(m_pSecondMomment != nullptr) delete m_pSecondMomment;
}

void AdamParamGroup::register_param(string param_name, 
                                    xt::xarray<double>* ptr_param,
                                    xt::xarray<double>* ptr_grad) {
    // Add the parameter and its gradient to the maps
    m_pParams->put(param_name,ptr_param);    
    m_pGrads->put(param_name,ptr_grad);
    
    // Initialize first and second moment estimates with zeros of the same shape as the parameter
    double_tensor* param_firstM = new xt::xarray<double>(xt::zeros_like(*ptr_param));
    double_tensor* param_secondM = new xt::xarray<double>(xt::zeros_like(*ptr_param));
    m_pFirstMomment->put(param_name,param_firstM);
    m_pSecondMomment->put(param_name,param_secondM);
}
void AdamParamGroup::register_sample_count(unsigned long long* pCounter){
    m_pCounter = pCounter;
}

void AdamParamGroup::zero_grad(){
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys){
        xt::xarray<double>* pGrad = m_pGrads->get(key);
        xt::xarray<double>* pParam = m_pParams->get(key);
        *pGrad = xt::zeros<double>(pParam->shape());
    }
    *m_pCounter = 0;
}
void AdamParamGroup::step(double lr) {
    // Lấy danh sách các khóa (tên tham số)
    DLinkedList<string> keys = m_pGrads->keys();
    
    for (auto key : keys) {
        xt::xarray<double>& param = *m_pParams->get(key);
        xt::xarray<double>& grad = *m_pGrads->get(key);

        xt::xarray<double>& firstMoment = *m_pFirstMomment->get(key);
        xt::xarray<double>& secondMoment = *m_pSecondMomment->get(key);

        firstMoment = m_beta1 * firstMoment + (1 - m_beta1) * grad;
        secondMoment = m_beta2 * secondMoment + (1 - m_beta2) * xt::square(grad);

        xt::xarray<double> firstMomentHat = firstMoment / (1 - m_beta1_t);
        xt::xarray<double> secondMomentHat = secondMoment / (1 - m_beta2_t);

        param -= lr * firstMomentHat / (xt::sqrt(secondMomentHat) + 1e-8);
    }
    m_step_idx += 1;
    m_beta1_t *= m_beta1;
    m_beta2_t *= m_beta2;
}
