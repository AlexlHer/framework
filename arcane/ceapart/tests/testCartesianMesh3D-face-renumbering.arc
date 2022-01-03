<?xml version="1.0"?>
<cas codename="ArcaneTest" xml:lang="fr" codeversion="1.0">
 <arcane>
  <titre>Test CartesianMesh</titre>

  <description>Test des maillages cartesiens</description>

  <boucle-en-temps>CartesianMeshTestLoop</boucle-en-temps>

  <modules>
    <module name="ArcanePostProcessing" active="true" />
  </modules>

 </arcane>

 <arcane-post-traitement>
   <periode-sortie>2</periode-sortie>
   <sauvegarde-initiale>true</sauvegarde-initiale>
   <depouillement>
    <variable>Density</variable>
    <groupe>AllCells</groupe>
   </depouillement>
 </arcane-post-traitement>
 
 <meshes>
   <mesh>
     <generator name="Cartesian3D">
       <nb-part-x>2</nb-part-x> 
       <nb-part-y>2</nb-part-y>
       <nb-part-z>1</nb-part-z>
       <origin>0.0 0.0 0.0</origin>
       <x><n>7</n><length>5.0</length></x>
       <y><n>5</n><length>3.0</length></y>
       <z><n>3</n><length>2.0</length></z>
     </generator>
   </mesh>
 </meshes>
 <cartesian-mesh-tester>
 </cartesian-mesh-tester>
</cas>
