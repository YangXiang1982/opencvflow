export enum PropertyType {
  Text,
  Integer,
  Decimal,
  Boolean,
  Choice,
  MultiChoice,
  BooleanMatrix,
  IntMatrix,
  DoubleMatrix
}

export declare type NodeProperty = {
  type: PropertyType;
  name: string;
  title?: string;
};
