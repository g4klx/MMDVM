# based on https://github.com/berndporr/iir_fixed_point/blob/master/gen_coeff.py

import numpy as np
import scipy.signal as signal
import pylab as pl

# Calculate the coefficients for a pure fixed point
# integer filter

# sampling rate
fs = 24000

# cutoffs
f1 = 300
f2 = 2700
# ripple
rp = 0.2

# scaling factor in bits, do not change !
q = 15
# scaling factor as facor...
scaling_factor = 2**q

# let's generate a sequence of 2nd order IIR filters
sos = signal.cheby1(3,rp,[f1, f2],'bandpass', output='sos', fs=fs)
#os = signal.cheby1(4, rp, f2, 'lowpass', output='sos', fs=fs) #deemphasis filter
#sos = signal.cheby1(1, rp, 2122, 'highpass', output='sos', fs=fs) #deemphasis filter

sosrounded = np.round((sos) * scaling_factor)

# print coefficients
for biquad in sosrounded:
    for coeff in biquad:
        print(int(coeff),",",sep="",end="")
        #print((coeff),",",sep="",end="")
    print("")

# plot the frequency response
b,a = signal.sos2tf(sos)
w,h = signal.freqz(b,a, worN=2048)
pl.plot(w/np.pi/2*fs,20*np.log(np.abs(h)))
pl.xlabel('frequency/Hz');
pl.ylabel('gain/dB');
pl.ylim(top=1,bottom=-30);
pl.xlim(left=250, right=12000);
pl.show()