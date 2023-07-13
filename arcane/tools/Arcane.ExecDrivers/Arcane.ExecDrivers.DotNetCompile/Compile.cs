//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.IO;
using Arcane.ExecDrivers.Common;

namespace Arcane.DotNetCompile
{
  class Compile
  {
    class Helper
    {
      internal List<string> m_args = new List<string>();

      internal void AddReferenceLib(string path, string name)
      {
        m_args.Add("/reference:" + Path.Combine(path, name));
      }
      internal void AddArg(string name)
      {
        m_args.Add(name);
      }
    }
    /*!
     * \brief Commande pour compiler les fichiers \a args.
     *
     * On utilise le compilateur 'Roslyn' associé à la version du SDK
     * avec laquelle on a compilé Arcane.
     */
    public int Execute(string[] args)
    {
      var helper = new Helper();
      string lib_dir = Utils.CodeLibPath;

      // Le répertoire de base de 'dotnet' est le répertoire dans lequel
      // il y a l'exécutable 'dotnet'
      string dotnet_command = Utils.DotnetCoreClrPath;
      string dotnet_full_version = Utils.DotnetCoreClrFullVersion;

      string dotnet_root = Path.GetDirectoryName(dotnet_command);
      string dotnet_sdk_path = Path.Combine(dotnet_root, "sdk", dotnet_full_version);

      // Le compilateur est: ${dotnet_root}/sdk/${dotnet_full_version}/Roslyn/bincore/csc.dll
      helper.AddArg(Utils.DotnetCoreClrPath);
      helper.AddArg("exec");
      helper.AddArg(Path.Combine(dotnet_sdk_path, "Roslyn", "bincore", "csc.dll"));
      helper.AddArg("/noconfig");
      helper.AddArg("/target:library");
      helper.AddReferenceLib(lib_dir, "Arcane.Core.dll");
      helper.AddReferenceLib(lib_dir, "Arcane.Utils.dll");
      helper.AddReferenceLib(Path.Combine(dotnet_sdk_path, "ref"), "netstandard.dll");
      helper.m_args.AddRange(args);
      string cmd = string.Join(" ", helper.m_args);
      Utils.ExecShellCommand(cmd, null);

      return 0;
    }
  }
}
