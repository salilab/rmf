# RMF Attributes and Decorators

# RMF Attributes and decorators # {#decoratorsattributes}

[TOC]

The attributes and decorators are divided into several categories, each associated with a different RMF::Category. The get and set functions on the decorators always have the same names as the attribute, unless otherwise noted. For example, RMF::decorator::Particle has RMF::decorator::Particle::get_mass().

## Physics ## {#physics}

The category name is `physics`. It includes information about the physical structure of molecules.

### Attributes ### {#physicsattributes}

| Name           | Type         | Description                               |
|---------------:|-------------:|:------------------------------------------|
| `coordinates`  | RMF::Vector3 |  The cartesian coordinates in angstroms   |
| `orientation`  | RMF::Vector4 |  The orientation quaternion               |
| `mass`         | float        |  The mass in Daltons                      |
| `radius`       | float        |  The radius in angstroms                  |
| `rotation`     | RMF::Vector4 |  The rotation quaternion                  |
| `translation`  | RMF::Vector3 |  The translation vector in angstroms      |
| `bonded {0,1}` | int          |  The RMF::NodeID of the bonds endpoints   |
| `element`      | int          |  The atomic number of the element         |
| `diffusion coefficient` | float | A diffusion coefficient in A^2/fs  |


### Decorators ### {#physicsdecorators}

| Name                            |  Node Type              | Attributes                         |
|--------------------------------:|:-----------------------:|:-----------------------------------|
| RMF::decorator::Particle        |  RMF::REPRESENTATION    | coordinates, mass, radius          |
| RMF::decorator::ReferenceFrame  |  any                    | rotation, translation              |
| RMF::decorator::Bond            |  RMF::BOND              | bonded {0,1}                       |
| RMF::decorator::Atom            |  RMF::REPRESENTATION    | coordinates, mass, radius, element |
| RMF::decorator::Diffuser        |  RMF::REPRESENTATION    | diffusion coefficient, coordinates |

## Sequence ## {#sequence}

The category name is `sequence` and it includes information about the types and indexes of residues.

### Attributes ### {#sequenceattributes}

| Name             | Type      | Description                                      |
|-----------------:|----------:|:-------------------------------------------------|
| `residue type`   | string    |  Three letter residue type                       |
| `residue index`  | int       |  The index of a residue                          |
| `chain id`       | string    |  The chain ID                                    |
| `first residue index` | int  |  The index of the first residue (in a domain)    |
| `last residue index` | int   |  The index of the last residue (in a domain)     |
| `residue indexes` | ints      |  The list of indexes in a fragment               |
| `copy index`     | int       |  The index of this particular copy of a protein (when there are multiple copies of the same protein) |
| `type name`      | string       |  An arbitrary type name for a particle. It could be the protein type or ligand type. |



### Decorators ### {#sequencedecorators}

| Name                            |  Node Type              | Attributes                         |
|--------------------------------:|:-----------------------:|:-----------------------------------|
| RMF::decorator::Residue         | RMF::REPRESENTATION     |  index, residue type               |
| RMF::decorator::Chain           | RMF::REPRESENTATION     |  chain id                          |
| RMF::decorator::Domain          | RMF::REPRESENTATION     |  first residue index, last residue index |
| RMF::decorator::Fragment        | RMF::REPRESENTATION     |  residue indexes                   |
| RMF::decorator::Copy            | RMF::REPRESENTATION     |  copy index                        |
| RMF::decorator::Typed           | RMF::REPRESENTATION     |  type name                         |


## Feature ## {#feature}

The category name is `feature` and the data stored relates to scoring functions, scores and which part of the structure they apply to.

### Attributes ### {#featureattributes}

| Name             | Type      | Description                                      |
|-----------------:|----------:|:-------------------------------------------------|
| `score`          | float     |  Some sort or score                              |
| `representation` | RMF::Ints |  The NodeIDs of the nodes involved in this score |

### Decorators ### {#featuredecorators}

| Name                            |  Node Type              | Attributes                         |
|--------------------------------:|:-----------------------:|:-----------------------------------|
| RMF::decorator::Score           | RMF::FEATURE            |  score                             |
| RMF::decorator::Representation  | RMF::FEATURE            |  representation                    |

## Shape ## {#shape}

The category name is `shape` and the information relates to geometric markup of the structure.

### Attributes ### {#shapeattributes}

| Name             | Type             | Description                                      |
|-----------------:|-----------------:|:-------------------------------------------------|
| `rgb color`      | RMF::Vector3     |  The red, green, and blue color components, each component is in the range from 0 to 1. |
| `coordinates list` | RMF::Vector3s  |  A list of coordinates.                          |
| `coordinates`    | RMF::Vector3     |  The coordinates in angstrom.                    |
| `radius`         | float            |  The radius in angstroms                         |
| `axis lengths`   | RMF::Vector3     |  The length along each coordinate axis.          |
| `orientation`    | RMF::Vector4     |  The ordientation quaternion.                    |

### Decorators ### {#shapedecorators}

| Name                            |  Node Type              | Attributes                         |
|--------------------------------:|:-----------------------:|:-----------------------------------|
| RMF::decorator::Colored         | any                     |  color                             |
| RMF::decorator::Ball            | RMF::GEOMETRY           |  coordinates, radius               |
| RMF::decorator::Segment         | RMF::GEOMETRY           |  coordinates list                  |
| RMF::decorator::Cylinder        | RMF::GEOMETRY           |  coordinates list, radius          |
| RMF::decorator::Ellipsoid       | RMF::GEOMETRY           |  coordinates, axis lengths, orientation |

## Alias ## {#alias}

The category name is `alias` and it stores information for nodes that provide a reference to another node, for example to provide an alternative organizational scheme.

### Attributes ### {#aliasattributes}

| Name             | Type      | Description                                      |
|-----------------:|----------:|:-------------------------------------------------|
| `alias`          | int       |  NodeID of the node being aliased                |

### Decorators ### {#aliasdecorators}

| Name                            |  Node Type              | Attributes                         |
|--------------------------------:|:-----------------------:|:-----------------------------------|
| RMF::decorator::Alias           | RMF::ALIAS              |  alias                             |

## External ## {#external}

The category name is `external` and it store references to all or part of external files. For example, a path to a PDB file that
should be loaded as a child of the current node.

### Attributes ### {#externalattributes}

| Name             | Type      | Description                                      |
|-----------------:|----------:|:-------------------------------------------------|
| `path`           | string    |  Relative path to the external file              |

### Decorators ### {#externaldecorators}

| Name                            |  Node Type              | Attributes                         |
|--------------------------------:|:-----------------------:|:-----------------------------------|
| RMF::decorator::External           | RMF::REPRESENTATION  |  path                              |


## Publication ## {#publication}

### Attributes ### {#publicationattributes}

| Name             | Type      | Description                                      |
|-----------------:|----------:|:-------------------------------------------------|
| `title`          | string    |  Title of the article                            |
| `journal`        | string    |  Title of the journal                            |
| `pubmed id`      | string    |  The pubmed ID                                   |
| `year`           | int       |  The year (full                                  |
| `authors`        | RMF::Strings |  The author names                                |


### Decorators ### {#publicationdecorators}

| Name                            |  Node Type              | Attributes                         |
|--------------------------------:|:-----------------------:|:-----------------------------------|
| RMF::decorator::JournalArticle  | RMF::ORGANIZATIONAL     |  title, journal, pubmed id, year, authors |
