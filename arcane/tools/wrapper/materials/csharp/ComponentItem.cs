using System.Runtime.InteropServices;
using System;
using Arcane;

namespace Arcane.Materials
{
  public unsafe interface IComponentItem
  {
    ComponentItemInternal* Internal { get; set; }
  }

  [StructLayout(LayoutKind.Sequential)]
  public unsafe struct ComponentItem : IComponentItem
  {
    internal ComponentItemInternal* m_internal;
    public ComponentItemInternal* Internal
    {
      get { return m_internal; }
      set { m_internal = value; }
    }

    public Cell GlobalCell { get { return new Cell(m_internal->m_global_item); } }
    public MatVarIndex MatVarIndex { get { return m_internal->m_var_index; } }
    internal int _matvarArrayIndex { get { return m_internal->m_var_index.ArrayIndex; } }
    internal int _matvarValueIndex { get { return m_internal->m_var_index.ValueIndex; } }
    public ComponentItem(ComponentItemInternal* ci)
    {
      m_internal = ci;
    }
  }

  [StructLayout(LayoutKind.Sequential)]
  public unsafe struct MatItem : IComponentItem
  {
    internal ComponentItemInternal* m_internal;
    public ComponentItemInternal* Internal
    {
      get { return m_internal; }
      set { m_internal = value; }
    }
    public Cell GlobalCell { get { return new Cell(m_internal->m_global_item); } }
    public MatVarIndex MatVarIndex { get { return m_internal->m_var_index; } }
    internal int _matvarArrayIndex { get { return m_internal->m_var_index.ArrayIndex; } }
    internal int _matvarValueIndex { get { return m_internal->m_var_index.ValueIndex; } }
    public MatItem(ComponentItemInternal* ci)
    {
      m_internal = ci;
    }
    public static implicit operator ComponentItem(MatItem c) => new ComponentItem(c.m_internal);
  }
  public unsafe struct EnvItem : IComponentItem
  {
    internal ComponentItemInternal* m_internal;
    public ComponentItemInternal* Internal
    {
      get { return m_internal; }
      set { m_internal = value; }
    }

    public Cell GlobalCell { get { return new Cell(m_internal->m_global_item); } }
    public MatVarIndex MatVarIndex { get { return m_internal->m_var_index; } }
    internal int _matvarArrayIndex { get { return m_internal->m_var_index.ArrayIndex; } }
    internal int _matvarValueIndex { get { return m_internal->m_var_index.ValueIndex; } }
    public EnvItem(ComponentItemInternal* ci)
    {
      m_internal = ci;
    }
    public static implicit operator ComponentItem(EnvItem c) => new ComponentItem(c.m_internal);
  }
}
