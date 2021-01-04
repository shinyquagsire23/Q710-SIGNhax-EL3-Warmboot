import hashlib

f = open("signtest-FFFFFFFF.bin", "rb")
f_out = open("signtest-gen.bin", "wb")
f_raw = open("raw_resources_a_mod.image", "rb")

h = hashlib.sha256()
h.update(f_raw.read())
f_raw.close()

f_out.write(f.read(0xc40))
f_out.write(h.digest()) # modified partition hash (A)
f.seek(0xC60)
f_out.write(f.read()) # modified partition hash (B)
f.close()
f_out.close()
