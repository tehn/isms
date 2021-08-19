SynthDef("PolyPerc", {
			arg out, freq = 440, pw=0.5, amp=0.5, cutoff=800, gain=1, release=0.9, pan=0;
			var snd = Pulse.ar(freq, pw);
			var filt = MoogFF.ar(snd,cutoff,gain);
			var env = Env.perc(level: amp, releaseTime: release).kr(2);
			Out.ar(out, Pan2.ar((filt*env), pan));
		}).add;

o = OSCFunc({ arg msg, time, addr, recvPort;
	Synth(\PolyPerc, [\freq, msg[1].midicps]);
}, '/n', n);