Sure, here's the contents for the file: /mapmaker/setup.py

from setuptools import setup, find_packages

setup(
    name='mapmaker',
    version='0.1',
    packages=find_packages(where='src'),
    package_dir={'': 'src'},
    install_requires=[
        'pygame',  # Example dependency for GUI
        'Pillow',  # For image handling
        'jsonschema'  # For JSON validation
    ],
    entry_points={
        'console_scripts': [
            'mapmaker=main:main',  # Entry point for the application
        ],
    },
    include_package_data=True,
    zip_safe=False,
)