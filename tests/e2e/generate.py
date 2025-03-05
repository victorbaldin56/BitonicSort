import os
import random
import config

MIN_NUMBER = -(1 << 31)
MAX_NUMBER = -(MIN_NUMBER + 1)

def generateRandomList():
  return [random.randint(MIN_NUMBER, MAX_NUMBER)
          for _ in range(random.randint(10000, 100000))]

def generateInputFile(test_num):
  inp = generateRandomList()
  input_str = str(len(inp)) + " " + " ".join([str(e) for e in inp]) + "\n"
  input_path = config.input_dir + f"test_{test_num + 1:02d}.in"
  with open(input_path, "w") as f:
    f.write(input_str)
  return inp

def generateAnsFile(test_num, inp):
  ans_path = config.ans_dir + f"ans_{test_num + 1:02d}.out"
  inp.sort()
  ans_str = " ".join([str(e) for e in inp]) + "\n"
  with open(ans_path, "w") as f:
    f.write(ans_str)

if not os.path.isdir(config.input_dir):
  os.makedirs(config.input_dir)

if not os.path.isdir(config.ans_dir):
  os.makedirs(config.ans_dir)

for test_num in range(config.NUM_TESTS):
  generateAnsFile(test_num, generateInputFile(test_num=test_num))
