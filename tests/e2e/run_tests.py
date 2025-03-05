import config
import subprocess

def getAns(input_file):
  with open(input_file, "r") as infile:
    process = subprocess.run(
      [config.file_path + "/../../build/bitonic"],
      stdin=infile, text=True, capture_output=True
    )

  if process.stderr != 0:
    raise RuntimeError(f"Bitonic driver failed: {process.stderr}")
  return process.stdout

def test(test_num):
  input_path = config.input_dir + f"test_{test_num:02d}.in"
  ans_path = config.ans_dir + f"ans_{test_num:02d}.out"

  ans_str = getAns(input_path)
  with open(ans_path, "r") as ans:
    reference_str = ans.readline()
    if (reference_str != ans_str):
      raise RuntimeError(f"Test {test_num} failed")
    print(f"Test {test_num} passed")

for test_num in range(1, 11):
  test(test_num)
