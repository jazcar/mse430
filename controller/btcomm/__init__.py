from sys import platform


__all__ = ['BTcomm']

if platform == 'linux':
    from btcomm.linux_socket import BTComm
elif platform == 'darwin':
    from btcomm.macos_serial import btcomm
else:
    raise ImportError('No bluetooth support for platform {}'.format(platform))
