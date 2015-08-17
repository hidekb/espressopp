#  Copyright (C) 2015
#      Jakub Krajniak (jkrajniak at gmail.com)
#
#  This file is part of ESPResSo++.
#
#  ESPResSo++ is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  ESPResSo++ is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.


r"""
*************************************
**espressopp.interaction.DihedralRB**
*************************************

The proper dihedral with Ryckaert-Bellemans form.

.. math::

    U_{rb}(\phi_{ijkl}) = \sum_{n=0}^{5} K_n (cos(\theta))^n

where the :math:`\theta = \phi - 180^\circ` and :math:`K_{0\dotso5}` are the coefficients.

By default the IUPAC convention is used, where :math:`\phi` is the angle between planes
:math:`ijk` and :math:`jkl`. The :math:`0^\circ` corresponds to the *cis* configuration.

Reference: http://www.gromacs.org/Documentation/Manual






.. function:: espressopp.interaction.DihedralRB(K0, K1, K2, K3, K4, K5, iupac)

		:param K0: (default: 0.0)
		:param K1: (default: 0.0)
		:param K2: (default: 0.0)
		:param K3: (default: 0.0)
		:param K4: (default: 0.0)
		:param K5: (default: 0.0)
		:param iupac: (default: True) If set to true then IUPAC convention for dihedrals is used (by default).
		:type K0: real
		:type K1: real
		:type K2: real
		:type K3: real
		:type K4: real
		:type K5: real
		:type iupac: Boolean 

.. function:: espressopp.interaction.FixedQuadrupleListDihedralRB(system, vl, potential)

		:param system: 
		:param vl: 
		:param potential: 
		:type system: 
		:type vl: 
		:type potential: 

.. function:: espressopp.interaction.FixedQuadrupleListDihedralRB.getFixedQuadrupleList()

		:rtype: FixedQuadrupleList

.. function:: espressopp.interaction.FixedQuadrupleListDihedralRB.setPotential(type1, type2, potential)

		:param type1: 
		:param type2: 
		:param potential: 
		:type type1: 
		:type type2: 
		:type potential: 

"""

from espressopp import pmi
from espressopp.esutil import *  # NOQA

from espressopp.interaction.DihedralPotential import *  # NOQA
from espressopp.interaction.Interaction import *  # NOQA
from _espressopp import interaction_DihedralRB
from _espressopp import interaction_FixedQuadrupleListDihedralRB


class DihedralRBLocal(DihedralPotentialLocal, interaction_DihedralRB):

    def __init__(self, K0=0.0, K1=0.0, K2=0.0, K3=0.0, K4=0.0, K5=0.0, iupac=True):
        if (not (pmi._PMIComm and pmi._PMIComm.isActive())
                or pmi._MPIcomm.rank in pmi._PMIComm.getMPIcpugroup()):
            if iupac:
                cxxinit(self, interaction_DihedralRB, K0, -1*K1, K2, -1*K3, K4, -1*K5)
            else:
                cxxinit(self, interaction_DihedralRB, K0, K1, K2, K3, K4, K5)


class FixedQuadrupleListDihedralRBLocal(InteractionLocal, interaction_FixedQuadrupleListDihedralRB):

    def __init__(self, system, vl, potential):
        if (not (pmi._PMIComm and pmi._PMIComm.isActive())
                or pmi._MPIcomm.rank in pmi._PMIComm.getMPIcpugroup()):
            cxxinit(self, interaction_FixedQuadrupleListDihedralRB, system, vl, potential)

    def setPotential(self, type1, type2, potential):
        if (not (pmi._PMIComm and pmi._PMIComm.isActive())
                or pmi._MPIcomm.rank in pmi._PMIComm.getMPIcpugroup()):
            self.cxxclass.setPotential(self, type1, type2, potential)

    def getFixedQuadrupleList(self):
        if (not (pmi._PMIComm and pmi._PMIComm.isActive())
                or pmi._MPIcomm.rank in pmi._PMIComm.getMPIcpugroup()):
            return self.cxxclass.getFixedQuadrupleList(self)

if pmi.isController:
    class DihedralRB(DihedralPotential):
        pmiproxydefs = dict(
            cls='espressopp.interaction.DihedralRBLocal'
            )

    class FixedQuadrupleListDihedralRB(Interaction):
        __metaclass__ = pmi.Proxy
        pmiproxydefs = dict(
            cls='espressopp.interaction.FixedQuadrupleListDihedralRBLocal',
            pmicall=['setPotential', 'getFixedQuadrupleList']
            )
