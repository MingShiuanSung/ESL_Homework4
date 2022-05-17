# ESL_Homework4

## Introduction
This project contains a Gaussion filter built on riscv-vp. <br>

## Build and run a gaussion filter on riscv-vp

1. Download the updated software source code to your working directory $EE6470. <br>

    <br>

        $ cd $EE6470 
    <br>

        $ git clone https://github.com/MingShiuanSung/ESL_Homework4.git
    <br>

        $ cp -r ESL_Homework4/riscv-vp/vp/src/platform/* $EE6470/riscv-vp/vp/src/platform
    <br>

        $ cp -r ESL_Homework4/riscv-vp/sw/* $EE6470/riscv-vp/sw
    <br>

2. Build the "basic-acc" platform of riscv-vp:

    <br>

        $ cd $EE6470
    <br>

        $ cd riscv-vp/vp/build
    <br>

        $ cmake ..
    <br>

        $ make install
    <br>

## Build Sobel filter software

<br>

    $ cd $EE6470
<br>

    $ cd riscv-vp/sw
<br>

    $ cd basic-gauss
<br>

    $ make
<br>

    $ make sim
<br>
