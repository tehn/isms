local midi = {}

midi.types = {
	note_on = 0x90,
	note_off = 0x80,
	cc = 0xb0,
}

midi.event = function(channel, type, data1, data2)
	print('midi event', channel, type, data1, data2)
end

midi.send = function(channel, type, data1, data2)
	channel = channel - 1
	status = type + channel
	isms.midi_send({status, data1, data2})
end

midi._event = function(data)
	status = data[1]

	channel = (status & 0x0F) + 1
	type = status & 0xF0

	data1 = data[2]
	data2 = data[3]

	midi.event(channel, type, data1, data2)
end

return midi
