# dde - dynamic derivative extractor

dde is a tool based on Intel Pin, which attempts to extract a DAG representation
of an executabe. A part of dde infrastructure will also be a post-processing
step, during which a graph representation is flattened into a source code extended
with AD routines. Namely, among those routines would be tangent and adjoint drivers,
as well as conjugate derivative routines.
