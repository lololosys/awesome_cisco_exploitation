import argparse
import sys
import zipfile

OFFSET_TO_E_MACHINE = 0x12
E_MACHINE_PPC_VALUE = '\x00\x14'

def _extract_zip_mem(zip_file):
    """
    :param zip_file:
        @zip_file - the path to the IOS binary file
    :return:
        The unzipped data in memory
    """
    try:
        zip_data = zipfile.ZipFile(zip_file)
    except:
        raise Exception('Unable to load the zip file specified')

    return {name: list(zip_data.read(name)) for name in zip_data.namelist()}

def _fix_ios_image(ios_images):
    """
    :param ios_images:
        @ios_images - The unzipped ios images in memory
    :return:
    """
    for ios_bin_file, ios_image in ios_images.items():
        print "Fixing %s..." %(ios_bin_file)
        ios_image[OFFSET_TO_E_MACHINE:OFFSET_TO_E_MACHINE+2] = E_MACHINE_PPC_VALUE

def _save_ios_disk(fixed_ios_images):
    """
    :param fixed_ios_images:
        @fixed_ios_images - The fixed ios images memory dump
    :return:
    """
    for ios_bin_file, ios_image in fixed_ios_images.items():
        try:
            with open(ios_bin_file, 'wb') as f:
                f.write("".join(ios_image))
        except:
            raise Exception('Failed to save the IOS image to disk')

def extract_ios_binary(zip_file):
    """
    :param zip_file:
        @zip_file - The path to the IOS binary file
    :return:
    """
    zip_data = _extract_zip_mem(zip_file)
    _fix_ios_image(zip_data)
    _save_ios_disk(zip_data)

def main():
    parser = argparse.ArgumentParser(description='IOS binary extraction tool')
    parser.add_argument('-i', '--input', help='IOS binary file')
    args = parser.parse_args()

    extract_ios_binary(args.input)
    print "Done!"

if __name__ == '__main__':
    sys.exit(main())
