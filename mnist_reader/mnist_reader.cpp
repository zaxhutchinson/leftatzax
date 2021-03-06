#include"mnist_reader.hpp"

MNISTReader::MNISTReader() {

}

void MNISTReader::LoadData(std::string & label_filename, std::string & images_filename) {


    std::ifstream ifs(label_filename, std::ios::binary);
    if(ifs) {
        ifs.read(reinterpret_cast<char*>(&labels_magic_number), 4);
        ifs.read(reinterpret_cast<char*>(&labels_size), 4);

        labels_magic_number = reverseInt(labels_magic_number);
        labels_size = reverseInt(labels_size);

        char label;
        while(!ifs.eof()) {
            ifs.read(&label,1);
            if(!ifs.fail()) {
                MNISTData md;
                md.label = static_cast<unsigned>(label);
                data.push_back(md);
            }
        }
    }
    ifs.close();

    ifs.open(images_filename, std::ios::binary);
    if(ifs) {

        ifs.read(reinterpret_cast<char*>(&images_magic_number), 4);
        ifs.read(reinterpret_cast<char*>(&images_size), 4);
        ifs.read(reinterpret_cast<char*>(&images_rows), 4);
        ifs.read(reinterpret_cast<char*>(&images_cols), 4);

        images_magic_number = reverseInt(images_magic_number);
        images_size = reverseInt(images_size);
        images_rows = reverseInt(images_rows);
        images_cols = reverseInt(images_cols);


        int i = 0;
        unsigned char pixel[1];
        while(!ifs.eof()) {
            vec<unsigned> image;
            for(int i = 0; i < images_rows*images_cols; i++) {
                ifs.read(reinterpret_cast<char*>(pixel),1);
                image.push_back(static_cast<unsigned>(*pixel));
            }
            if(!ifs.fail()) {
                data[i].image=std::move(image);
                i++;
            }
                
        }
    }
    ifs.close();

}

vec<MNISTData> MNISTReader::GetDataWithLabel(unsigned label) {
    vec<MNISTData> d;
    for(unsigned i = 0; i < data.size(); i++) {
        if(label==data[i].label) {
            d.push_back(data[i]);
        }
    }
    return d;
}

vec<MNISTData> & MNISTReader::GetData() {
    return data;
}

MNISTData * MNISTReader::GetDataAt(unsigned i) {
    try {
        return &(data.at(i));
    } catch(std::out_of_range e) {
        return nullptr;
    }
}

void MNISTReader::ToPPM(unsigned i) {
    MNISTData * md = GetDataAt(i);
    if(md) {
        std::ofstream ofs("image_"+std::to_string(i)+".ppm");
        ofs << "P2" << std::endl;
        ofs << images_rows << " " << images_cols << std::endl;
        ofs << 255;

        for(unsigned i = 0; i < md->image.size(); i++) {
            if(i%images_cols==0 && i < md->image.size()-1) ofs << std::endl;
            else ofs << " ";
            ofs << md->image[i];
        }
        ofs.close();
    }
}

int32_t MNISTReader::reverseInt (int32_t i) 
{
    uint8_t c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int32_t)c1 << 24) + ((int32_t)c2 << 16) + ((int32_t)c3 << 8) + c4;
}

void MNISTReader::Shuffle() {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::shuffle(data.begin(), data.end(), rng);
}

void MNISTReader::PrintStats() {
    std::cout << "The next three values should be identical.\n";
    std::cout << "# Labels: " << labels_size << std::endl;
    std::cout << "# Images: " << images_size << std::endl;
    std::cout << "# Data:   " << data.size() << std::endl;

    std::map<unsigned,unsigned> count;
    
    for(int i = 0; i < data.size(); i++) {
        unsigned cur = 0;
        try {
            cur = count.at(data[i].label);
        } catch(std::out_of_range e) {}
        count.insert_or_assign(data[i].label,cur+1);
    }


    unsigned total = 0;
    for(std::map<unsigned,unsigned>::iterator it = count.begin(); 
        it != count.end(); it++) {

        std::cout << it->first << ": " << it->second << std::endl;
        total+= it->second;    
    }
    std::cout << "TOTAL: " << total << std::endl;
    
    if(labels_size==images_size && images_size==data.size() && data.size()==total) {
        std::cout << "Sizes match.\n";
    } else {
        std::cout << "Size mismatch.\n";
    }

}