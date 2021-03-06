/* flace*/
#include <getopt.h>
#include<ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "metadata.hpp"
#include "wavereader.hpp"
#include "bitwriter.hpp"
#include "flacencoder.hpp"


void exit_with_help(char *argv[]){
    fprintf(stderr, "usage: %s [ OPTIONS ] infile.flac [outfile.wav]\n", argv[0]);
    fprintf(stderr, "  -f : Use fixed frames\n");
    fprintf(stderr, "  -v : Encode verbatim (no compression)\n");
    fprintf(stderr, "  -s=order : Encode single frame with order given\n");
    exit(1);
}

int main(int argc, char *argv[]){
    int opt = 0, fixed = 0, verbatim = 0, single = 0, encode = 0;
    int order = 0;
    int print_residuals = 0;
    while ((opt = getopt(argc,argv,"fvs:")) != EOF)
        switch(opt)
        {
            case 'f': fixed = 1; encode = 1; break;
            case 'v': verbatim = 1; encode = 1; break;
            case 's': single = 1; 
                      encode = 1; 
                      std::cerr << "OPTARG::: " << optarg << "\n";
                      if (strcmp(optarg, "v") == 0)
                          verbatim = 1;
                      else
                        order = atoi(optarg);
                      break;
            case 'h':
            case '?': 
            default:
                exit_with_help(argv);
        }
        
    std::shared_ptr<std::fstream> fin;
    std::shared_ptr<std::fstream> fout;
    
    if (optind == 1) exit_with_help(argv);
    
    if (optind < argc){
        fin = std::make_shared<std::fstream>(argv[optind], std::ios::in | std::ios::binary);
        if(fin->fail()) {
            fprintf(stderr, "ERROR: opening %s for input\n", argv[optind]);
            return 1;
        }
    }
        
    if (encode){
        fout = std::make_shared<std::fstream>(argv[optind + 1], std::ios::out | std::ios::binary);
        if(fout->fail()) {
            fprintf(stderr, "ERROR: opening %s for output\n", argv[optind + 1]);
            return 1;
        }
    }
    
    
    auto fr = std::make_shared<BitReader>(fin);
    WaveReader *wr = new WaveReader();
    wr->read_metadata(fr);
    WaveMetaData *meta = wr->getMetaData();
    meta->print(stdout);
    
    int spb = 4096;
    
    int16_t pcm[spb];
    int32_t pcm32[spb];
    unsigned i;
    
    if (fixed){
        
        fprintf(stdout, "%ld samples to encode\n", meta->getNumSamples());
        auto fe = FLACEncoder(fout);
        fe.setSamples(meta->getNumSamples());
        fe.write_header();
        
        double total_samples = meta->getNumSamples();
        
        for (i = 0; i + spb < meta->getNumSamples(); i += spb){
            wr->read_data(fr, pcm, spb);
            for (unsigned j = 0; j < spb; j++){ pcm32[j] = (int32_t) pcm[j];}
            fe.write_frame(pcm32, spb, i/spb);
            
            printf("%.2f%% Encoded\n", ((double) i)/total_samples * 100);
        }
        
        /*if (i != meta->getNumSamples()){
            int remainder = meta->getNumSamples() - i;
            wr->read_data(fr, pcm, remainder);
            for (unsigned j = 0; j < remainder; j++){ pcm32[j] = (int32_t) pcm[j];}
            
        }*/
    } else if (single){
        auto fe = FLACEncoder(fout);
        
        wr->read_data(fr, pcm, spb);
        for (unsigned j = 0; j < spb; j++){ pcm32[j] = (int32_t) pcm[j];}
        
        if (verbatim){
            fe.write_frame_verbatim(pcm32, 4096, 0);
        } else {
            fe.write_frame_fixed(pcm32, 4096, order, 0);
        }
    }
    
}