import procedural_3d_terrain.models.cprogan as cp
import procedural_3d_terrain.test_models.progan as p
import argparse
import torch as th
from torchvision.utils import save_image
import os
import numpy as np
import redis
import json

DEPTH = 7
LATENT_SIZE = 256
NUM_CLASS = 3

CPGAN_MODEL_DIR = 'three_env/models/'
PGAN_MODEL_DIR = 'yoca_models/models/'

class RedisQueue(object):
    """
        Redis Lists are an ordered list, First In First Out Queue
        Redis List pushing new elements on the head (on the left) of the list.
        The max length of a list is 4,294,967,295
    """
    def __init__(self, name, **redis_kwargs):
        """
            host='localhost', port=6379, db=0
        """
        self.key = name
        self.rq = redis.Redis(**redis_kwargs)
        
    def flush_all(self):
        self.rq.flushall() 

    def size(self): # 큐 크기 확인
        return self.rq.llen(self.key)

    def isEmpty(self): # 비어있는 큐인지 확인
        return self.size() == 0

    def put(self, element): # 데이터 넣기
        self.rq.lpush(self.key, element) # left push

    def get(self, isBlocking=False, timeout=None): # 데이터 꺼내기
        if isBlocking:
            element = self.rq.brpop(self.key, timeout=timeout) # blocking right pop
            element = element[1] # key[0], value[1]
        else:
            element = self.rq.rpop(self.key) # right pop
        return element

    def get_without_pop(self): # 꺼낼 데이터 조회
        if self.isEmpty():
            return None
        element = self.rq.lindex(self.key, -1)
        return element

    def set_value(self, key, value):
        self.rq.set(key, value)

    def clear(self):
        self.rq.delete(self.key)

def process_cpgan():
    input_noize = th.randn(sample_num, LATENT_SIZE).to(device)
    input_label = None
    if grid_size == 1:
        input_label = th.as_tensor([label]).to(device)
    else:
        input_label = th.as_tensor([i%NUM_CLASS for i in range(sample_num)]).to(device)

    sample = cpgan.print(input_noize, input_label, start_depth, alpha= alpha)
    save_image(sample, os.path.join(save_path, file_name), nrow=int(np.sqrt(len(sample))),
                   normalize=True, scale_each=True)
    print("save image")
    return 0

def process_pgan():

    return 0

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='cProGANs or ProGans')
    parser.add_argument('--label', type=int ,help='label', default=0)
    parser.add_argument('--save_path', type=str,default='./')
    parser.add_argument('--save_file_name', type=str,default='result.png')
    parser.add_argument('--grid_size', type=int, default=1)
    parser.add_argument('--start_depth', type=int, default=5)
    parser.add_argument('--models_output_path', type=str, default='./procedural_3d_terrain/output')
    

    args = parser.parse_args()

    label = args.label
    save_path = args.save_path
    grid_size = args.grid_size
    start_depth = args.start_depth
    file_name = args.save_file_name
    models_output_path = args.models_output_path
    alpha = 1.0


    sample_num = grid_size*grid_size
    device = th.device('cuda') if th.cuda.is_available() else th.device('cpu')

    cpgan = cp.ConditionalProGAN(num_classes=NUM_CLASS, depth=DEPTH, latent_size=LATENT_SIZE, device = device)
    cpgan.reload(start_depth= start_depth, save_dir=os.path.join(models_output_path, CPGAN_MODEL_DIR))

    # r = redis.Redis(host="127.0.0.1", port=6379)
    q = RedisQueue('mq', host="127.0.0.1", port=6379, db=0)
    q.clear()
    q.set_value("IDLE", 1)
    q.set_value("UPDATE", 0)
    while(True):
        msg = q.get(isBlocking=True)
        try:
            if msg is not None:
                msg_json = json.loads(msg.decode('utf-8'))
                print("get: ", msg_json)
                if "label" in msg_json:
                    label = int(msg_json["label"])
                    print(label)
                if "grid_size" in msg_json:
                    grid_size = int(msg_json["grid_size"])
                    sample_num = grid_size*grid_size
                if "alpha" in msg_json:
                    alpha = float(msg_json["alpha"])

                if "CGAN" in msg_json:
                    print("start gan")
                    q.set_value("IDLE", 0)
                    process_cpgan()
                    q.set_value("IDLE", 1)
                    q.set_value("UPDATE", 1)
                    print("end gan")
                if "save_path" in msg_json:
                    save_path = msg_json["save_path"]
                if "model_output_path" in msg_json:
                    model_output_path = msg_json["model_output_path"]
                if "start_depth" in msg_json:
                    start_depth  = int(msg_json["start_depth"])
                if "file_name" in msg_json:
                    file_name = msg_json["file_name"]

                if "end" in msg_json:
                    break
            else:
                break
        except Exception as e:
            print(e)
        
    print("exit")
    q.flush_all()



