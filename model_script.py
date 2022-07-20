import procedural_3d_terrain.models.cprogan as cp
import procedural_3d_terrain.test_models.progan as p
import argparse
import torch as th
from torchvision.utils import save_image
import os
import numpy as np

DEPTH = 7
LATENT_SIZE = 256
NUM_CLASS = 3

CPGAN_MODEL_DIR = 'd3_env copy 4/models/'
PGAN_MODEL_DIR = 'yoca_models/models/'

def process_cpgan():
    gan = cp.ConditionalProGAN(num_classes=NUM_CLASS, depth=DEPTH, latent_size=LATENT_SIZE, device = device)
    gan.reload(start_depth= start_depth, save_dir=os.path.join(models_output_path, CPGAN_MODEL_DIR))
    
    input_noize = th.randn(sample_num, LATENT_SIZE).to(device)
    input_label = None
    if grid_size == 1:
        input_label = th.as_tensor([label]).to(device)
    else:
        input_label = th.as_tensor([i%NUM_CLASS for i in range(sample_num)]).to(device)

    sample = gan.print(input_noize, input_label, start_depth, 1)
    save_image(sample, os.path.join(save_path, file_name), nrow=int(np.sqrt(len(sample))),
                   normalize=True, scale_each=True)
    return 0

def process_pgan():

    return 0

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='cProGANs or ProGans')
    parser.add_argument('--model', type=str, default='cp', help='cp or p')
    parser.add_argument('--label', type=int ,help='label', default=0)
    parser.add_argument('--save_path', type=str,default='./')
    parser.add_argument('--save_file_name', type=str,default='result.png')
    parser.add_argument('--grid_size', type=int, default=1)
    parser.add_argument('--start_depth', type=int, default=5)
    parser.add_argument('--models_output_path', type=str, default='./procedural_3d_terrain/output')
    

    args = parser.parse_args()
    model_name = args.model
    label = args.label
    save_path = args.save_path
    grid_size = args.grid_size
    start_depth = args.start_depth
    file_name = args.save_file_name
    sample_num = grid_size*grid_size
    models_output_path = args.models_output_path


    device = th.device('cuda') if th.cuda.is_available() else th.device('cpu')

    if model_name == 'cp':
        process_cpgan()
    else:
        process_pgan()





