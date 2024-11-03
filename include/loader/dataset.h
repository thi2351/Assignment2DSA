/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataset.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 3:59 PM
 */

#ifndef DATASET_H
#define DATASET_H
#include "tensor/xtensor_lib.h"
using namespace std;

template<typename DType, typename LType>
class DataLabel{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
public:
    DataLabel(xt::xarray<DType> data,  xt::xarray<LType> label):
    data(data), label(label){
    }
    xt::xarray<DType> getData() const{ return data; }
    xt::xarray<LType> getLabel() const{ return label; }
};

template<typename DType, typename LType>
class Batch{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
public:
    Batch(xt::xarray<DType> data,  xt::xarray<LType> label):
    data(data), label(label){
    }
    virtual ~Batch(){}
    xt::xarray<DType>& getData(){return data; }
    xt::xarray<LType>& getLabel(){
        if (label.shape().size() == 0) {
            static xt::xarray<LType> empty_label = xt::xarray<LType>(0);
            return empty_label;
        }
        return label; }
};

template<typename DType, typename LType>
class Dataset{
private:
public:
    Dataset(){};
    virtual ~Dataset(){};
    
    virtual int len()=0;
    virtual DataLabel<DType, LType> getitem(int index)=0;
    virtual xt::svector<unsigned long> get_data_shape()=0;
    virtual xt::svector<unsigned long> get_label_shape()=0;
    
};

//////////////////////////////////////////////////////////////////////
template<typename DType, typename LType>
class TensorDataset: public Dataset<DType, LType> {
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;
    

public:
    /* TensorDataset: 
     * cần khởi tạo:
     * 1. data, label;
     * 2. data_shape, label_shape
    */
    bool has_label; 
    TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label = xt::xarray<LType>()) 
        : data(data), label(label) {
        data_shape = xt::svector<unsigned long>(data.shape().begin(), data.shape().end());

        if (label.shape().size() == 0) {
            label_shape = xt::svector<unsigned long>();
            has_label = false;
        }
        else {
            label_shape = xt::svector<unsigned long>(label.shape().begin(), label.shape().end());
            has_label = true;
        }
    }

    /* len():
     *  trả về kích thước của chiều đầu tiên (dữ liệu)
    */
    int len() {
        return (int)(data_shape[0]);
    }

    /* getitem:
     * trả về một đối tượng DataLabel cho chỉ số được chỉ định
     */
    DataLabel<DType, LType> getitem(int index) {
        if (index < 0 || index >= data.shape()[0]) {
            throw std::out_of_range("Index is out of range");
        }

        auto data_item = xt::view(data, index, xt::all());
        xt::xarray<LType> label_item;
        if (has_label) {
            label_item = xt::view(label, index, xt::all());
        } 
        return DataLabel<DType, LType>(data_item, label_item);
    }

    xt::svector<unsigned long> get_data_shape() {
        return data_shape;
    }

    xt::svector<unsigned long> get_label_shape() {
        return label_shape;
    }

    /* Kiểm tra xem dataset có nhãn hay không */
    bool hasLabels() const {
        return has_label;
    }
};




#endif /* DATASET_H */

