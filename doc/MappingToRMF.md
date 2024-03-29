Mapping data to RMF files {#mapping}
=========================

This page outlines how one would go about storing various types of data in RMF files. Much of what it says is merely suggestions, and is not mandatory.

For nodes types, the following node shapes are used
\dot
 digraph key {
   root [label="RMF::ROOT" shape=circle URL="\ref RMF::ROOT"];
   rep [label="RMF::REPRESENTATION" shape=oval URL="\ref RMF::REPRESENTATION"];
   bond [label="RMF::BOND" shape=invhouse URL="\ref RMF::BOND"];
   org [label="RMF::ORGANIZATIONAL" shape=folder URL="\ref RMF::ORGANIZATIONAL"];
   feat [label="RMF::FEATURE" shape=hexagon URL="\ref RMF::FEATURE"];
   tree [label="elided subtree" shape=triangle];
    }
\enddot
And a dot means an RMF::ALIAS node. The key concept to keep in mind with RMF::ALIAS nodes is that, unlike everything else, they don't introduce a new entity, simply point to one that exists elsewhere.
See RMF::ROOT, RMF::REPRESENTATION, RMF::BOND, RMF::ORGANIZATIONAL, RMF::PROVENANCE, RMF::FEATURE for more information.

# PDB files #

PDB files are most naturally mapped as follows:
- each PDB chain is a node that is a child of the root and stores data using the RMF::decorator::Chain decorator
- for a protein, each residue is a node that is the child of the respective chain and stores data using the RMF::decorator::Residue decorator
- each atom is a node that is the child of its residue or molecule. It stores data using the RMF::decorator::Atom and RMF::decorator::Particle decorators

All bonds are stored as RMF::BOND nodes with two children for the endpoints of the bond. The bonds are stored as children of the first common ancestor of the endpoints.

\dot
 digraph pdb {
      root [label="root" shape=circle];
      chain [label="Chain A" shape=folder];
      r0 [label="Residue 0 (RMF::decorator::Residue)" shape=folder URL="\ref RMF::decorator::Residue"];
      r1 [label="Residue 1 (RMF::decorator::Residue)" shape=folder URL="\ref RMF::decorator::Residue"];
      br [label="bond" shape=invhouse];
      br0 [label="bonded" shape=point];
      br1 [label="bonded" shape=point];
      rdot [label="Residue ... (RMF::decorator::Residue)" shape=folder URL="\ref RMF::decorator::Residue"];
      a0 [label="N (RMF::decorator::Atom, RMF::decorator::Particle)" shape=oval URL="\ref RMF::decorator::Atom"];
      a1 [label="CA (RMF::decorator::Atom, RMF::decorator::Particle)" shape=oval URL="\ref RMF::decorator::Atom"];
      a2 [label="C (RMF::decorator::Atom, RMF::decorator::Particle)" shape=oval URL="\ref RMF::decorator::Atom"];
      ap0 [label="N (RMF::decorator::Atom, RMF::decorator::Particle)" shape=oval URL="\ref RMF::decorator::Atom"];
      apdot [label="... (RMF::decorator::Atom, RMF::decorator::Particle)" shape=oval URL="\ref RMF::decorator::Atom"];
      b0 [label="bond" shape=invhouse];
      b00 [label="bonded" shape=point];
      b01 [label="bonded" shape=point];
      b1 [label="bond" shape=invhouse];
      b10 [label="bonded" shape=point];
      b11 [label="bonded" shape=point];
      root -> chain;
      chain->r0;
      chain->br;
      chain->r1;
      chain->rdot;
      r0->a0;
      r0->a1;
      r0->a2;
      br->br0;
      br0->a2
      br->br1;
      br1->ap0;
      r0->b0;
      b0->b00;
      b00->a0;
      b0->b01;
      b01->a1;
      r0->b1;
      b1->b10;
      b10->a1;
      b1->b11;
      b11->a2;
      r1->ap0;
      r1->apdot;
    }
\enddot

If the PDB file has more than one model, all with the same structure, the models should be stored as frames in the RMF file.

# Rigid fits of proteins into EM maps or other data #

When you have a number of structures that are formed by applying a rigid transform to a set of base structures (eg fitting pdb structures into an EM map), they can be stored as follows:
- RMF::decorator::ReferenceFrame nodes can be used to store the rigid transforms
- under each RMF::decorator::ReferenceFrame node, you can store one of
     - a RMF::decorator::External referencing an external structure
     - an RMF hierarchy detailing the structure being transformed
Multiple RMF::decorator::External or the child hierarchies can refer to the same structure. Hopefully software loading the RMF file will take advantage of that.

Multiple structures can be stored as separate frames.

\dot
 digraph rigid {
      root [label="root" shape=circle];
      copy_0 [label="Copy 0 (RMF::decorator::ReferenceFrame)" shape=folder URL="\ref RMF::decorator::ReferenceFrame"];
      copy_1 [label="Copy 1 (RMF::decorator::ReferenceFrame)" shape=folder URL="\ref RMF::decorator::ReferenceFrame"];
      copy_2 [label="Copy 2 (RMF::decorator::ReferenceFrame)" shape=folder URL="\ref RMF::decorator::ReferenceFrame"];
      pdb [label="External PDB (RMF::decorator::External)" shape=oval URL="\ref RMF::decorator::External"];
      em [label="External MRC (RMF::decorator::External)" shape=oval URL="\ref RMF::decorator::External"];
      root -> copy_0;
      root -> copy_1;
      root -> copy_2;
      root->em;
      copy_0 -> pdb;
      copy_1 -> pdb;
      copy_2 -> pdb;
    }
\enddot

# Structures with multiple organization schemes #

If the file contains data that has multiple organization schemes, for example, grouping by type of protein, which unit cell etc, several different hierarchies can be provided, all with the same child structures using RMF::ALIAS nodes to provide the different organization schemes.

\dot
 digraph symmetry {
      root [label="root" shape=circle];
      unit_cell [label="structure" shape=folder];
      a [label="Protein A" shape=triangle];
      b [label="Protein B" shape=triangle];
      c [label="Protein C" shape=triangle];
      d [label="Protein D" shape=triangle];
      bt [label="By fold" shape=folder];
      bth [label="Beta barrel" shape=folder];
      btx [label="Alpha helical" shape=folder];
      bta [label="Protein A" shape=point];
      btb [label="Protein B" shape=point];
      btc [label="Protein C" shape=point];
      btd [label="Protein D" shape=point];
      root->unit_cell;
      unit_cell->a;
      unit_cell->b;
      unit_cell->c;
      unit_cell->d;
      root->bt;
      bt->btx;
      bt->bth;
      btx->bta;
      btx->btb;
      bth->btc;
      bth->btd;
      bta->a;
      btb->b;
      btc->c;
      btd->d;
    }
\enddot

# Symmetries #

When there is symmetry, or other repeated unit, define the unit cell in one sub hierarchy and then add RMF::decorator::ReferenceFrame nodes that have that unit cell as a child to define the symmetric parts.

\dot
 digraph symmetry {
      root [label="root" shape=circle];
      unit_cell [label="unit cell" shape=folder];
      structure [label="structural nodes" shape=triangle];
      copy_0 [label="Copy 0 (RMF::decorator::ReferenceFrame)" shape=folder URL="\ref RMF::decorator::ReferenceFrame"];
      copy_1 [label="Copy 1 (RMF::decorator::ReferenceFrame)" shape=folder URL="\ref RMF::decorator::ReferenceFrame"];
      copy_2 [label="Copy 2 (RMF::decorator::ReferenceFrame)" shape=folder URL="\ref RMF::decorator::ReferenceFrame"];
      root -> unit_cell;
      unit_cell -> structure
      root -> copy_0;
      copy_0 -> structure;
      root -> copy_1;
      copy_1 -> structure;
      root -> copy_2;
      copy_2 -> structure;
    }
\enddot
