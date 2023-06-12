import shutil
from pathlib import Path
from waflib.Build import BuildContext

class install_lib_class(BuildContext):
    cmd = 'install-lib'
    fun = 'install_lib'

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    conf.load('compiler_c')

    try:
        conf.check_cfg(package='lua54', args='--cflags --libs', uselib_store='LUA')
    except conf.errors.ConfigurationError:
        conf.check_cfg(package='lua', args='lua >= 5.4 --cflags --libs', uselib_store='LUA')

    conf.check_cfg(msg='Checking for SDL2',
        package='',
        path='sdl2-config',
        args='--cflags --libs',
        uselib_store='SDL2')

    conf.check_cfg(package='liblo', args='--cflags --libs', uselib_store='LO')
    conf.check_cc(lib='pthread', uselib_store='PTHREAD')
    conf.check_cc(lib='m', uselib_store='M')

def build(bld):
    print(bld)
    bld.program(source=bld.path.ant_glob('src/**/*.c'),
        includes=['src', 'src/clock'],
        target='isms',
        use=['LO', 'LUA', 'SDL2', 'PTHREAD', 'M'])

def install_lib(ctx):
    src = Path(ctx.path.abspath()) / 'lua'
    dst = Path.home() / '.local' / 'share' / 'isms' / 'system'

    dst.mkdir(parents=True, exist_ok=True)
    shutil.copytree(src, dst, dirs_exist_ok=True)
