import argparse
import os

import build_runner

abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
original_cwd = os.getcwd()

ENGINE_ASSETS_DIR = os.path.join(dname, '../../assets')
GAME_ASSETS_DIR = os.path.join(original_cwd, 'assets')
DST_DIR = os.path.join(original_cwd, '_built_assets')
CACHE_DIR = os.path.join(original_cwd, '_assets_cache')

MODEL_COMPILER = os.path.join(dname, 'model_compiler.exe')
SHADER_COMPILER = os.path.join(dname, 'render_program_compiler.exe')

def main():
  parser = argparse.ArgumentParser(description='Run an asset build for an slt target.')
  args = parser.parse_args()

  os.makedirs(CACHE_DIR, exist_ok=True)
  os.makedirs(DST_DIR, exist_ok=True)

  runner = build_runner.Runner(CACHE_DIR)

  print('using model compiler:     ' + MODEL_COMPILER)
  print('using shader compiler:    ' + SHADER_COMPILER)
  print('using engine assets root: ' + ENGINE_ASSETS_DIR)
  print('using game assets root:   ' + GAME_ASSETS_DIR)
  print('outputing to:             ' + DST_DIR)

  print('building engine assets...')
  buildAssetsFrom(ENGINE_ASSETS_DIR, runner)
  
  print('build game assets...')
  buildAssetsFrom(GAME_ASSETS_DIR, runner)

  return 0

def getAssets(src_dir, extension):
  result = []
  for root, dirs, files in os.walk(src_dir):
      for file in files:
        if file.endswith(extension):
          src = os.path.join(root, file).replace("\\","/")
          result.append(src)

  return result

def buildAssetsFrom(src_dir, runner):
  os.chdir(src_dir)
  print('  build models...')
  models = getAssets('models', '.fbx')
  for m in models:
    dst = os.path.join(DST_DIR, m[:-4])

    os.makedirs(dst, exist_ok=True)
    runner.Build(MODEL_COMPILER, {
      'input': os.path.join(src_dir, m),
      'output': dst
      }, 
      src_dir,
      m)
  runner.WaitForCompletion()

  print('  build shaders...')
  shaders = getAssets('render_programs', '.prog')
  for m in shaders:
    dst = os.path.join(DST_DIR, m)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    runner.Build(SHADER_COMPILER, {
      'input': os.path.join(src_dir, m),
      'output': dst
      }, 
      src_dir,
      m)
  
  print('  build screens...')
  screens = getAssets('screens', '')
  for m in screens:
    dst = os.path.join(DST_DIR, m)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    runner.Copy(src=os.path.join(src_dir, m), dst=dst, cwd=src_dir)
  runner.WaitForCompletion()

  

if __name__ == '__main__':
  main()