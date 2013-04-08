#!/usr/bin/python

# TODO: test dynamic hook of synchronization routines

# TODO: to check the determinism of the RR scheduler, perhaps just run the
# test program a few times and check that the generated schedules are
# identical.

import re
import os
import sys
import errno
import shutil
import filecmp
import argparse
import subprocess

parser = argparse.ArgumentParser(description
                                 ='run scheduler test');
parser.add_argument('program',
                    help='.c, .cpp, or .ll program to run test on')
parser.add_argument('-gxx', dest='gxx', required=True,
                    help='g++ command line')
parser.add_argument('-llvmgcc', dest='llvmgcc', required=True,
                    help='llvmgcc command line')
parser.add_argument('-projbindir', dest='projbindir', required=True,
                    help='directory where tern tools are located')
parser.add_argument('-ternruntime', dest='ternruntime', required=True,
                    help='tern x86 runtime')
parser.add_argument('-ternannotlib', dest='ternannotlib', required=True,
                    help='tern annotation library')
parser.add_argument('-ternbcruntime', dest='ternbcruntime', required=True,
                    help='tern bc runtime')
parser.add_argument('-nondet', dest='nondet', default=False, action='store_true',
                    help='skip checking of determinism')
parser.add_argument('-gen', dest='gen', default=False, action='store_true',
                    help='generate expected outputs instead of testing them')

def gen(cmd, prog):
    m = re.search('\|\s*FileCheck.*$', cmd)
    if m == None:
        return False
    print 'generating outputs for %s on %s' % (cmd, prog)
    cmd = cmd[:m.start(0)]
    #print cmd
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
    output = p.communicate()[0];

    fi = open(prog, 'r')
    fo = open(prog + '~', 'w')
    # print until the first matching lines
    for l in fi:
        if re.match('//\s*CHECK', l):
            break
        fo.write(l)
    fo.write('// CHECK indicates expected output checked by FileCheck; '\
             'auto-generated by appending -gen to the RUN command above.\n')
    for i, l in enumerate(output.splitlines()):
        l = re.split('\s*0x[\dA-Fa-f]+', l, maxsplit=1)[0]
        if i == 0:
            s = ':     '
        else:
            s = '-NEXT:'
        fo.write("// CHECK%s %s\n" % (s, l))
    # skip lines matching @prefix and print other lines
    for l in fi:
        if re.match('//\s*CHECK', l):
            continue
        fo.write(l)
    fo.close()
    fi.close()
    os.rename(fo.name, fi.name)
    return True

class ptrmap:
    def __init__(self):
        self.map = dict()
        self.num = 0
    def get(self, k, create=0):
        if k not in self.map:
            if not create:
                return None
            self.map[k] = 'ptr' + str(self.num)
            self.num += 1
        return self.map[k]
    def rem(self, k):
        del self.map[k]

def normalize_ptr(kvmap, m):
    k = int(m.group(), 16)
    limit = 10
    if k >= -limit and k <= limit:
        return str(k)
    if k == errno.ETIMEDOUT or k == errno.EBUSY:
        return str(k)
    return kvmap.get(k, create=1)

def remove_destroyed(kvmap, l):
    destroy_list = ["pthread_join",
                    "pthread_mutex_destroy",
                    "pthread_barrier_destroy",
                    "pthread_cond_destroy",
                    "sem_destroy"]
    for s in destroy_list:
        if s not in l:
            continue
        m = re.search('0x[\dA-Fa-f]+', l)
        if m == None:
            continue
        k = int(m.group(), 16)
        if kvmap.get(k) != None:
            kvmap.rem(k)

curry = lambda func, *args, **kw: \
    lambda *p, **n: func(*args + p, **dict(kw.items() + n.items()))

def check_schedule(schedule):
    # check that schedule is not empty
    assert os.path.getsize(schedule) > 0
    # TODO: check that no critical sections of the same lock overlap
    # TODO: check that semaphore count is always non-negative

def normalize_schedule(schedule):
    kvmap = ptrmap()
    pat = re.compile('0x[\dA-Fa-f]+')
    fi = open(schedule, 'r')
    fo = open(schedule + '~', 'w')
    for l in fi:
        lo = pat.sub(curry(normalize_ptr, kvmap), l)
        remove_destroyed(kvmap, l)
        fo.write(lo)
    fo.close()
    fi.close()
    os.rename(fo.name, fi.name)

def check_deterministic(cmd, prog):
    m = re.search('output_dir=([^\s:]+)[\s:]', cmd)
    assert m != None
    print 'output directory is %s' % m.group(1)
    output_dir = m.group(1)

    # generate schedule
    schedule = output_dir.replace('outdir', 'schedule');
    shutil.rmtree(output_dir)
    os.system(cmd)
    os.system('sort -k 2 -n %s/*.txt > %s' % (output_dir, schedule))
    normalize_schedule(schedule)
    check_schedule(schedule)

    # check determinism
    for i in range(10):
        new_schedule = ('%s.%d' % (schedule, i))
        shutil.rmtree(output_dir)
        os.system(cmd)
        os.system('sort -k 2 -n %s/*.txt > %s' % (output_dir, new_schedule))
        normalize_schedule(new_schedule)
        if not filecmp.cmp(schedule, new_schedule):
            print 'schedule %s and %s differ!' % (schedule, new_schedule)
            os.system('diff %s %s' % (schedule, new_schedule))
            exit(1)
    return

def run(cmd, map):
    cmd = cmd.split('RUN:')
    if len(cmd) == 1:
        return
    cmd = cmd[1]
    for key, val in map.iteritems():
        # print key, val
        if isinstance(val, str):
            cmd = cmd.replace('%'+key, val)
    if args['gen'] and gen(cmd, prog):
        return
    if (not args['nondet']) and cmd.endswith('ScheduleCheck'):
        cmd = cmd.split('ScheduleCheck')[0]
        check_deterministic(cmd, prog)
        return
    print 'running <%s>' % cmd
    os.system(cmd);

args = vars(parser.parse_args())
prog = args['program']
del args['program']

args['s'] = prog
args['t'] = os.path.basename(args['s']) + '.tmp'

# these commands follow the same style as LLVM dejagnu test commands
cmds = '''

// test dynamic hooking
// RUN: %gxx -o %t4 %s -lpthread -lrt %ternannotlib
// test FCFS scheduler
// NOTE: do not use dync_geteip as the lock used by backtrace() may cause
// a deadlock

// test RR scheduler
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 | FileCheck %s
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 ScheduleCheck

// test RR scheduler
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:nanosec_per_turn=100000:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 | FileCheck %s
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:nanosec_per_turn=100000:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 ScheduleCheck
'''

if os.getenv('test_dync_only') != None :
  cmds = '''
// test dynamic hooking
// RUN: %gxx -o %t4 %s -lpthread -lrt %ternannotlib
// test FCFS scheduler
// NOTE: do not use dync_geteip as the lock used by backtrace() may cause
// a deadlock

// test RR scheduler
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:nanosec_per_turn=100000:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 | FileCheck %s
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:nanosec_per_turn=100000:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 ScheduleCheck

// test RR scheduler
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 | FileCheck %s
// RUN: env TERN_OPTIONS=set_mutex_errorcheck=1:dync_geteip=0:log_type=test:exec_sleep=0:output_dir=%t2.outdir:enforce_turn_type=1:log_sync=1:dync_geteip=1  LD_PRELOAD=$XTERN_ROOT/dync_hook/interpose.so  ./%t4 ScheduleCheck
'''

for cmd in cmds.splitlines():
    run(cmd, args)
