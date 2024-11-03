/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "tensor/xtensor_lib.h"
#include "loader/dataset.h"

using namespace std;

template<typename DType, typename LType>
class DataLoader{
public:
    class Iterator; //forward declaration for class Iterator
    
private:
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int nbatch;
    ulong_tensor item_indices;
    int m_seed;
    
public:
    DataLoader(Dataset<DType, LType>* ptr_dataset, 
            int batch_size, bool shuffle=true, 
            bool drop_last=false, int seed=-1)
                : ptr_dataset(ptr_dataset), 
                batch_size(batch_size), 
                shuffle(shuffle),
                m_seed(seed){
            nbatch = ptr_dataset->len()/batch_size;
            item_indices = xt::arange(0, ptr_dataset->len());
    }
    virtual ~DataLoader(){}
    
    //New method: from V2: begin
    int get_batch_size(){ return batch_size; }
    int get_sample_count(){ return ptr_dataset->len(); }
    int get_total_batch(){return int(ptr_dataset->len()/batch_size); }
    
    //New method: from V2: end
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// START: Section                                                     //
    /////////////////////////////////////////////////////////////////////////
public:
    class Iterator {
    private:
        DataLoader<DType, LType>* loader;
        int batch_index;
    public:
        Iterator(DataLoader<DType, LType>* loader, int batch_index)
            : loader(loader), batch_index(batch_index) {}

        bool operator!=(const Iterator &other) {
            return this->batch_index != other.batch_index;
        }

        Iterator& operator++() {
            this->batch_index++;
            return *this;
        }

        Iterator operator++(int) {
            Iterator toStore = *this;
            this->batch_index++;
            return toStore;
        }
        Batch<DType, LType> operator*() {
    int start = batch_index * loader->batch_size;
    int end = min(start + loader->batch_size, this->loader->get_sample_count()); //Xử lí đoạn trước
    if (end + loader->batch_size > this->loader->get_sample_count()) {
        //Xử lí batch cuối
        if (loader->drop_last) {
            //Giữ nguyên
        }
        else {
            end = this->loader->get_sample_count();
        }
    }
    xt::svector<unsigned long> batch_datashape = loader->ptr_dataset->get_data_shape();
    xt::svector<unsigned long> batch_labelshape = loader->ptr_dataset->get_label_shape();
    batch_datashape[0] = end - start;
    batch_labelshape[0] = end - start;
    xt::xarray<DType> batch_data = xt::empty<DType>(batch_datashape);
    xt::xarray<LType> batch_label = xt::empty<LType>(batch_labelshape);
    for (int i = start; i < end; i++) {
        DataLabel<DType,LType> data = loader->ptr_dataset->getitem(loader->item_indices[i]);
        xt::view(batch_data, i - start, xt::all()) = data.getData();
        if (loader->ptr_dataset->get_label_shape().size() != 0) {
        xt::view(batch_label, i - start, xt::all()) = data.getLabel();
        }
    }
    return Batch<DType, LType>(batch_data, batch_label);
}
    };
    Iterator begin(){
        return Iterator(this,0);
    }
    Iterator end(){
        return Iterator(this,nbatch);
    }
    
    //BEGIN of Iterator

    //YOUR CODE IS HERE: to define iterator

    //END of Iterator
    
    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// END: Section                                                       //
    /////////////////////////////////////////////////////////////////////////
};


#endif /* DATALOADER_H */

