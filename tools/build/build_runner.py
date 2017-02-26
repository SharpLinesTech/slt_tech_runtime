# build running and caching.
# There is no paralelization for the moment, but adding it should be pretty simple.

import subprocess as proc
import hashlib
import os
import json
import time

def _ArgsDictToList(args):
  return ['--' + name + '=' + val for name, val in args.items()]

def GetHash(cmds, cwd):
  m = hashlib.sha256()
  m.update(cwd.encode('utf-8'))
  for cmd in cmds:
    for c in cmd:
      m.update(c.encode('utf-8'))

  return m.hexdigest()

class Runner:
  def __init__(self, cache_dir):
    self._cache_dir = cache_dir

  def Copy(self, src, dst, cwd):
    cmd_1 = ['cmake', '-E', 'copy_if_different', src.replace("/","\\"), dst.replace("/","\\")]
    cmd_2 = ['cmake', '-E', 'touch', dst.replace("/","\\")]
    self.Exec([cmd_1, cmd_2], cwd, src, [src], [dst])

  def Build(self, tool, args, cwd, src):
    cmd = [tool] + _ArgsDictToList(args)
    self.Exec([cmd], cwd, src, [tool], [])
  
  def Exec(self, cmds, cwd, src, inputs, outputs):
    os.chdir(cwd)
    
    should_run = False
    cache_file_name = GetHash(cmds, cwd) + '.txt'
    cache_file = os.path.join(self._cache_dir, cache_file_name)

    print('    Building: ' + src + ' as: ' + cache_file_name)

    cache = {
      "inputs" : [],
      "outputs" : [],
      "timestamp" : 0,
      "success" : False,
      "stdout" : [],
      "stderr" : [],
      "cwd" : "",
      "cmd" : []
    }
    should_run = False

    if os.path.isfile(cache_file):
      with open(cache_file) as cache_data:
        cache = json.load(cache_data)

      if cache['success'] == False:
        print('      Last run failed')
        should_run = True

      for i in cache['inputs']:
        if os.path.isfile(i):
          ts = os.path.getmtime(i)
          if ts > cache['timestamp']:
            print('      ' + i + ' has been modified')
            should_run = True
        else:
          print('      ' + i + ' could not be found')
          should_run = True

      for i in cache['outputs']:
        if os.path.isfile(i):
          ts = os.path.getmtime(i)
          if ts < cache['timestamp']:
            print('      ' + i + ' is stale')
            should_run = True
        else:
          print('      ' + i + ' could not be found')
          should_run = True

    else:
      print("      no cache found")
      should_run = True

    if should_run:
      cache['timestamp'] = time.time()
      cache['cmd'] = [ ' '.join(cmd) for cmd in cmds ]
      cache['cwd'] = cwd
      cache['inputs'] = [__file__]
      cache['outputs'] = []
      
      cache['inputs'].extend(inputs)
      cache['outputs'].extend(outputs)

      cache['stdout'] = []
      cache['stderr'] = []

      cache['success'] = True
      for cmd in cmds:
        print('      Executing: ' + ' '.join(cmd))
        result = proc.run(cmd, stdout=proc.PIPE, stderr=proc.PIPE, encoding='utf-8')

        out_strings = list(filter(None, result.stdout.split('\n')))
        err_strings = list(filter(None, result.stderr.split('\n')))
        cache['stdout'].append(out_strings)
        cache['stderr'].append(err_strings)
        cache['success'] &= result.returncode == 0

        # Extract read/write files.
        for line in err_strings:
          read_file = line.split('Reading from file: ', 1)
          if len(read_file) == 2:
            cache['inputs'].append(os.path.join(cwd, read_file[1]))
          write_file = line.split('Writing to file: ', 1)
          if len(write_file) == 2:
            cache['outputs'].append(os.path.join(cwd, write_file[1]))

        if result.returncode != 0:
          print("Failure")

      cache['inputs'] = list(set(cache['inputs']))
      cache['outputs'] = list(set(cache['outputs']))
      with open(cache_file, 'w') as outfile:
        json.dump(cache, outfile, sort_keys=True, indent=2)

  def WaitForCompletion(self):
    pass