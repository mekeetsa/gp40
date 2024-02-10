#!/usr/bin/env python3
"""
usage: wav2gtp.py [-h] [-v] [-c CHANNEL] [-t] [--oformat OFORMAT] [--hi HI]
                  [--low LOW] [--gap GAP] [--zero ZERO] [--one ONE]
                  input_file [output_file]

Decodes a Galaksija tape wav file.
Writes the output in either GTP or RAW file format.
Supported wav samples are int32, int16, uint8 and float32.

positional arguments:
  input_file            input file name
  [output_file]         output file name, optional; by default input file.wav
                        becomes file.gtp

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbosity       increase output verbosity
  -q, --quiet           quiet (set verbosity to none)
  -t, --test            test the checksum and quit; default = False
  --oformat {gtp|raw}   output format; default: gtp
  -c CHANNEL, --channel CHANNEL
                        audio channel; default: 0 (auto)
  --hi HI               hystresis high-value for detection of a rising edge;
                        default: 0.1
  --low LOW             hysteresis low-value for detection of a rising edge;
                        default: -0.1
  --gap GAP             min gap between the octets (milliseconds); default:
                        3.2 ms
  --zero ZERO           min period between the rising edges for "0"
                        (milliseconds); default: 2.5 ms
  --one ONE             max period between the rising edges for "1"
                        (milliseconds); default: 2.1 ms
"""

###############################################################################
# Parse the arguments
###############################################################################

import argparse

parser = argparse.ArgumentParser(
  description='''Decodes a Galaksija tape wav file.
Writes the output in either GTP or RAW file format.
Supported wav samples are int32, int16, uint8 and float32.''')

parser.add_argument("-v", "--verbosity", 
  action="count", default=0,
  help="increase output verbosity")

parser.add_argument("-q", "--quiet", 
  action="count", default=0,
  help="quiet (set verbosity to none)")

parser.add_argument('input_file',
  default='',
  help='input file name')

parser.add_argument('output_file', nargs='?',
  default='',
  help='output file name; by default input file.wav becomes output file.gtp')

parser.add_argument('-c', '--channel', type=int,
  default=0,
  help='audio channel; default: 1')

parser.add_argument('-t', '--test', action="count", default=0,
  help='test the checksum and quit; default = False')

parser.add_argument('--oformat', type=str, choices=['gtp', 'raw'],
  default='gtp',
  help='output format, either raw or gtp; default: gtp')

parser.add_argument('--hi', type=float,
  default=0.1,
  help='hysteresis high-value for detection of a rising edge; default: 0.1')

parser.add_argument('--low', type=float,
  default=-0.1,
  help='hysteresis low-value for detection of a rising edge; default: -0.1')

parser.add_argument('--gap', type=float,
  default=3.0,
  help='min gap between the octets (milliseconds); default: 3.0 ms')

parser.add_argument('--zero', type=float,
  default=2.5,
  help='min period between the rising edges for "0" (milliseconds); default: 2.5 ms')

parser.add_argument('--one', type=float,
  default=2.1,
  help='max period between the rising edges for "1" (milliseconds); default: 2.1 ms')

args = parser.parse_args()

if args.quiet > 0:
    args.verbosity = -1

import os

if args.output_file == '':
    base, ext = os.path.splitext(args.input_file)
    if args.oformat == 'gtp':
        args.output_file = base + ".gtp"
    elif args.oformat == 'raw':
        args.output_file = base + ".raw"
    
if args.verbosity >= 1:
    print(f"""Arguments:
    input file:     {args.input_file}
    output file:    {args.output_file}
    verbosity:      {args.verbosity}
    hysteresis hi:  {args.hi}
    hysteresis low: {args.low}
    threshold gap:  {args.gap}
    threshold zero: {args.zero}
    threshold one:  {args.one}""")

##############################################################################
# Read audio samples from a wave file
###############################################################################

import scipy.io.wavfile as wf
import warnings

with warnings.catch_warnings():
    warnings.simplefilter('ignore', wf.WavFileWarning)
    sample_rate, samples = wf.read(args.input_file)

sample_count = samples.shape[0]
number_of_channels = 1 if len(samples.shape) < 2 else samples.shape[1]

if args.verbosity >= 0:
    # ch_info = 'auto' if args.channel == 0 else str(args.channel)
    print(f"""Input WAV:
    File:           {args.input_file}
    Channels:       {number_of_channels}
    Data type:      {samples.dtype}
    Sample rate:    {sample_rate} Hz
    Length:         {sample_count} samples ({sample_count/sample_rate:.3f} s)""")

###############################################################################
# Decode audio samples recorded in the Galaksija tape format.
###############################################################################

def decode_galaksija_tape(samples):
    """
    Parameters
    ----------
    samples : float
        Wav samples (accepted: int32, int16, uint8, float32 and float64)

    Returns
    -------
    int array:
        Decoded octets (or [] in case of an error).
    """

    # Normalize data to float -1.0..+1.0

    if samples.dtype == 'int32':
        data = samples / ( 1.0 + 2**(32 - 1) )
    elif samples.dtype == 'int16':
        data = samples / ( 1.0 + 2**(16 - 1) )
    elif samples.dtype == 'uint8':
        data = samples / 127.0 - 1.0
    elif samples.dtype == 'float32':
        data = samples
    elif samples.dtype == 'float64':
        data = samples
    else:
        print(f"\nUnsupported encoding {samples.dtype}.\n")
        return []

    state = 0
    last_t = 0
    rising_edges = ""
    found_error = False

    # We first detects the rising edges of the signal
    # then measure (decode) the period between them.
    # The rising_edges string contains:
    #     '_ |' = a gap
    #     '_|'  = a short pause
    #     '|'   = a very short pause
    #     'E(<n>)' = an error at sample <n> (cannot decide if it is '0' or '1')
    
    for i in range(len(data)):
        if state == 0 and data[i] > args.hi:
            state = 1
            ts = round( 10000.0 * (i - last_t) / sample_rate ) / 10.0
            if ts >= args.gap:
                rising_edges = rising_edges + "_ |"  # long pause (a gap)
            elif ts >= args.zero:
                rising_edges = rising_edges + "_|"   # short pause ('0')
            elif ts <= args.one:
                rising_edges = rising_edges + "|"    # very short pause ('1')
            else:
                # Error: something between '0' and '1'
                found_error = True
                rising_edges = rising_edges + "E(" + str(i) + ")"
            last_t = i
        elif state == 1 and data[i] < args.low:
            state = 0
                    
    rising_edges = rising_edges + "__" # additional gap at the end
  
    if found_error:
        print("\nRising edges:", rising_edges)
        print("\nError: neither 0 nor 1 detected in the stream.\n")
        return []

    if args.verbosity >= 3:
        print("\nRising edges:", rising_edges)

    bits = ( rising_edges.
        replace("||", "1").  # Consecutive two short rising_edges are '1'  
        replace("|_", "0").  # Longer pause is '0'
        replace("1_", "1")   # '1' followed by a gap is just '1'
    )
    
    if args.verbosity >= 3:
        print("\nDetected bits:", bits)

    bits = ( bits.
        replace("_", "").    # Remove the rest of gaps
        replace(" ", "")     # Remove the rest of gaps
    )

    octets = []
    found_magic_octet = False
    magic_octet = 0xA5  # Leading octets before the magic are ignored
    leader = []
   
    for i in range(0, len(bits), 8):
        
        # Note that bits in an octet are LSB first so we need to reverse them.
        octet = int(bits[i:i+8][::-1], 2)

        # Skip the leading zeros until a magic octet is found
        if octet == magic_octet:
            found_magic_octet = True
        if found_magic_octet:
            octets.append(octet)
        elif octet == 0:
            leader.append(octet)
        else:
            leader.append(octet)
            print("Warning: Ignored a non-zero lead:", octet)
            
    # Format (ref. Tomaz Solc, Replika mikroracunalnika Galaksija): 
    # 0xA5, START ADDR (16-bit), END ADDR + 1 (16-bit), data, CKSUM, TRAILER 

    octets_len = len(octets)
    if octets_len < 5:
        print("Data stream has no header.")
        return []

    mem_addr1 = octets[1] + octets[2] * 256
    mem_addr2 = octets[3] + octets[4] * 256
    mem_len = mem_addr2 - mem_addr1

    data_len = mem_len + 1 + 2 + 2 + 1
    trailer_len = len(octets) - data_len
    octetsum = sum(octets[:data_len]) % 0x100 # without the trailer
    checksum = octets[data_len-1]

    is_OK = octetsum == 0xFF
    is_BASIC = mem_addr1 == 0x2C36
    checksum_warning = "" if is_OK else ", WARNING: Not 0xFF!" 

    if trailer_len < 0:
        print("Data stream is too short.")
        return []
    elif trailer_len == 0:
        trailer = []
    else:
        trailer = octets[data_len:]

    if trailer_len <= 3:
        trailer_info = f"{[ '0x{:02x}'.format(i) for i in trailer ]}".replace("'","")
    else:
        trailer_info = (
           f"[ 0x{trailer[0]:02x}, 0x{trailer[1]:02x}, 0x{trailer[2]:02x}, ... ] " +
           f"({trailer_len} bytes)" )

    if args.verbosity >= 0:
        print(f"""Data stream:
    Length:         0x{len(octets):04x} ({len(octets):d} bytes)
    Magic:          0x{octets[0]:02x}
    Leader:         {len(leader)} bytes
    Trailer:        {trailer_info}
    Mem start:      0x{mem_addr1:04x}
    Mem end+1:      0x{mem_addr2:04x}
    Mem len:        0x{mem_len:04x} ({mem_len:d} bytes)
    BASIC?          {is_BASIC}
    Checksum:       0x{checksum:02x}
    Octetsum:       0x{octetsum:02x}{checksum_warning}
    Octetsum OK?    {is_OK}""")

    if args.test:
        if is_OK:
            print(f"{args.input_file} checksum OK,",
                  f"leader_len = {len(leader)}, trailer_len = {trailer_len}")
            exit(0)
        else:
            print(f"{args.input_file} checksum ERROR 0x{octetsum:02x},",
                  f"leader_len = {len(leader)}, trailer_len = {trailer_len}")
            exit(-1)

    return octets

###############################################################################
# Decode either a specific channel or go through all the channels
###############################################################################

if args.channel >= 1 and args.channel <= number_of_channels:
    # Specific channel
    if number_of_channels == 1:
        octets = decode_galaksija_tape(samples)
    else:
        octets = decode_galaksija_tape(samples[:,args.channel - 1])
elif number_of_channels == 1:
    # Automatic channel and number_of_channels == 1
    octets = decode_galaksija_tape(samples)
else:
    # Automatic channel and number_of_channels > 1
    for c in range(number_of_channels):
        if args.verbosity >= 0:
            print(f"    Using channel:  {c+1}")
        octets = decode_galaksija_tape(samples[:,c])
        if octets != []:
            break

if octets == []:
    exit(-1)

###############################################################################
# Save the output
###############################################################################

import struct

# Save the decoded octets as-is (in the RAW format)

if args.oformat == 'raw':

    raw_binary = struct.pack("B"*len(octets), *octets)
    with open(args.output_file, "wb") as binary_file:
        binary_file.write(raw_binary)

    if args.verbosity >= 0:
        print(f"""Output RAW:
    File:           {args.output_file}
    Length:         0x{len(raw_binary):04x} ({len(raw_binary):d} bytes)""")

# Encapsulate the decoded octets in the GTP format

if args.oformat == 'gtp':
    gtp_header = [0x00, len(octets) % 256, len(octets) // 256, 0x00, 0x00]
    gtp = gtp_header + octets

    if args.verbosity >= 2:
        print("\nGTP:", ["{:02x}".format(i) for i in gtp])

    gtp_binary = struct.pack("B"*len(gtp), *gtp)
    with open(args.output_file, "wb") as binary_file:
        binary_file.write(gtp_binary)

    if args.verbosity >= 0:
        print(f"""Output GTP:
    File:           {args.output_file}
    Length:         0x{len(gtp_binary):04x} ({len(gtp_binary):d} bytes)""")

