<?xml version="1.0"?>
<cas codename="ArcaneTest" xml:lang="fr" codeversion="1.0">
 <arcane>
  <titre>Test CartesianMesh</titre>

  <description>Test des maillages cartesiens AMR 2D</description>

  <boucle-en-temps>AMRCartesianMeshTestLoop</boucle-en-temps>

  <modules>
    <module name="ArcanePostProcessing" active="true" />
    <module name="ArcaneCheckpoint" active="true" />
  </modules>

 </arcane>

 <arcane-post-traitement>
   <periode-sortie>1</periode-sortie>
   <depouillement>
    <variable>Density</variable>
    <variable>NodeDensity</variable>
    <groupe>AllCells</groupe>
    <groupe>AllNodes</groupe>
    <groupe>AllFacesDirection0</groupe>
    <groupe>AllFacesDirection1</groupe>
   </depouillement>
 </arcane-post-traitement>
 
 
 <maillage amr="true">
   <meshgenerator>
     <cartesian>
       <nsd>2 2 1</nsd>
       <origine>0.0 0.0 0.0</origine>
       <lx nx='3' prx='1.1'>3.0</lx>
       <lx nx='5' prx='1.0'>5.0</lx>

       <ly ny='3' pry='1.0'>3.0</ly>
       <ly ny='5' pry='1.1'>10.0</ly>

       <lz nz='4' prz='1.0'>2.0</lz>
     </cartesian>
   </meshgenerator>
 </maillage>

 <a-m-r-cartesian-mesh-tester>
   <renumber-patch-method>1</renumber-patch-method>
   <coarse-at-init>true</coarse-at-init>
   <refinement-3d>
     <position>1.0 2.0 0.5</position>
     <length>1.0 2.0 1.0</length>
   </refinement-3d>
   <refinement-3d>
     <position>1.4 3.0 1.0</position>
     <length>0.5 1.0 0.5</length>
   </refinement-3d>
   <expected-number-of-cells-in-patchs>32 256 32 16</expected-number-of-cells-in-patchs>
   <expected-number-of-ghost-cells-in-patchs>40 320 32 16</expected-number-of-ghost-cells-in-patchs>
   <nodes-uid-hash>f708721c983630feaa936c31f1d839dc</nodes-uid-hash>
   <faces-uid-hash>ec4598f6ca14d980bd3bee7e4959787f</faces-uid-hash>
   <cells-uid-hash>6a466175634dd458394e1d44a488f908</cells-uid-hash>
 </a-m-r-cartesian-mesh-tester>

 <arcane-protections-reprises>
   <service-protection name="ArcaneBasic2CheckpointWriter" />
 </arcane-protections-reprises>
</cas>
