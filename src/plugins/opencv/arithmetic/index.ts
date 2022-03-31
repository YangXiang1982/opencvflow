import cv, { Mat } from 'opencv-ts';
import { NormTypes } from 'opencv-ts/src/core/CoreArray';
import { DataTypes } from 'opencv-ts/src/core/HalInterface';
import { Position } from 'react-flow-renderer';
import {
  CVFOutputComponent,
  CVFIOEndlessComponent,
  CVFComponent,
  CVFIOComponent,
} from 'renderer/types/component';
import { SourceHandle, TargetHandle } from 'renderer/types/handle';
import { CVFNodeProcessor } from 'renderer/types/node';
import { PropertyType } from 'renderer/types/property';

const tabName = 'Arithmetic';

export class CVPlusComponent extends CVFComponent {
  static menu = { tabTitle: tabName, title: '+' };
  targets: TargetHandle[] = [
    { title: 'src1', position: Position.Left },
    { title: 'src2', position: Position.Left },
    { title: 'masc', position: Position.Left },
  ];
  sources: SourceHandle[] = [{ title: 'out', position: Position.Right }];

  static processor = class PlusProcessor extends CVFNodeProcessor {
    async proccess() {
      const { inputs } = this;
      if (inputs.length > 1) {
        const [src1, src2, masc] = inputs;
        const out: Mat = new cv.Mat(
          src1.rows,
          src1.cols,
          src1.type(),
          new cv.Scalar(0)
        );

        if (src1 && src2) {
          if (masc) {
            cv.add(src1, src2, out, masc);
          } else if (inputs.length === 2) {
            cv.add(src1, src2, out);
          }

          this.output(out);
          this.sources = [out];
        }
        return;
      }
      this.sources = [];
    }
  };
}

export class CVSubComponent extends CVFComponent {
  static menu = { tabTitle: tabName, title: '-' };
  targets: TargetHandle[] = [
    { title: 'src1', position: Position.Left },
    { title: 'src2', position: Position.Left },
    { title: 'masc', position: Position.Left },
  ];
  sources: SourceHandle[] = [{ title: 'out', position: Position.Right }];

  static processor = class SubProcessor extends CVFNodeProcessor {
    async proccess() {
      const { inputs } = this;
      if (inputs.length > 1) {
        const [src1, src2, masc] = inputs;
        const out: Mat = new cv.Mat(
          src1.rows,
          src1.cols,
          src1.type(),
          new cv.Scalar(0)
        );

        if (src1 && src2) {
          if (masc) {
            cv.subtract(src1, src2, out, masc);
          } else if (inputs.length === 2) {
            cv.subtract(src1, src2, out);
          }

          this.output(out);
          this.sources = [out];
        }
        return;
      }
      this.sources = [];
    }
  };
}

export class CVMultiplyComponent extends CVFIOEndlessComponent {
  static menu = { tabTitle: tabName, title: '*' };
  static processor = class MultiplyProcessor extends CVFNodeProcessor {
    async proccess() {
      let out: Mat | null = null;

      const { inputs } = this;
      if (inputs.length) {
        this.sources = [];
        for (const src of inputs) {
          if (!out) {
            out = src.clone();
          } else {
            cv.multiply(out, src, out, 1);
          }
        }
      }

      if (out) {
        this.output(out);
        this.sources = [out];
      } else {
        this.sources = [];
      }
    }
  };
}

export class CVDivisionComponent extends CVFIOEndlessComponent {
  static menu = { tabTitle: tabName, title: '/' };
  static processor = class DivisionProcessor extends CVFNodeProcessor {
    async proccess() {
      let out: Mat | null = null;

      const { inputs } = this;
      if (inputs.length) {
        this.sources = [];
        for (const src of inputs) {
          if (!out) {
            out = src.clone();
          } else {
            cv.divide(out, src, out, 1);
          }
        }
      }

      if (out) {
        this.output(out);
        this.sources = [out];
      } else {
        this.sources = [];
      }
    }
  };
}

export class CVMulComponent extends CVFIOEndlessComponent {
  static menu = { tabTitle: tabName, title: 'Mul' };
  static processor = class MulProcessor extends CVFNodeProcessor {
    async proccess() {
      let out: Mat | null = null;

      const { inputs } = this;
      if (inputs.length) {
        this.sources = [];
        for (const src of inputs) {
          if (!out) {
            out = src.clone();
          } else {
            out = out.mul(src);
          }
        }
      }

      if (out) {
        this.output(out);
        this.sources = [out];
      } else {
        this.sources = [];
      }
    }
  };
}

export class CVKernelComponent extends CVFOutputComponent {
  static menu = { tabTitle: tabName, title: 'Kernel' };

  static processor = class KernelProcessor extends CVFNodeProcessor {
    static properties = [{ name: 'kernel', type: PropertyType.DoubleMatrix }];

    kernel: Mat = new cv.Mat(3, 3, cv.CV_64F, new cv.Scalar(0));

    async proccess() {
      this.output(this.kernel!);

      this.sources = [this.kernel!];
    }
  };
}

export class CVGaussianKernelComponent extends CVFOutputComponent {
  static menu = { tabTitle: tabName, title: 'GausKernel' };

  static processor = class GaussianKernelProcessor extends CVFNodeProcessor {
    static properties = [
      { name: 'sigma', type: PropertyType.Decimal },
      { name: 'rows', type: PropertyType.Integer },
      { name: 'cols', type: PropertyType.Integer },
    ];

    sigma: number = 1;
    rows: number = 5;
    cols: number = 5;
    kernel?: Mat;

    buildKernel() {
      this.kernel = new cv.Mat(
        this.rows,
        this.cols,
        cv.CV_32F,
        new cv.Scalar(0)
      );

      cv.multiply(
        cv.getGaussianKernel(this.rows, this.sigma, cv.CV_32F),
        cv.getGaussianKernel(this.cols, this.sigma, cv.CV_32F).t(),
        this.kernel,
        1
      );
    }

    async proccess() {
      if (
        !this.kernel ||
        this.kernel.rows !== this.rows ||
        this.kernel.cols !== this.cols
      ) {
        this.buildKernel();
      }

      this.output(this.kernel!);

      this.sources = [this.kernel!];
    }
  };
}

export class CVNormalizeComponent extends CVFIOComponent {
  static menu = { tabTitle: tabName, title: 'Normalize' };

  static processor = class NormalizeProcessor extends CVFNodeProcessor {
    static properties = [
      { name: 'alpha', type: PropertyType.Integer },
      { name: 'beta', type: PropertyType.Integer },
      { name: 'normType', type: PropertyType.Integer },
      { name: 'dtype', type: PropertyType.Integer },
    ];

    alpha: number = 1;
    beta: number = 0;
    normType: NormTypes = cv.NORM_INF;
    dtype?: DataTypes;

    async proccess() {
      const { inputs } = this;
      if (inputs.length) {
        this.sources = [];
        for (const src of inputs) {
          const out = new cv.Mat(src.rows, src.cols, src.type());
          cv.normalize(
            src,
            out,
            this.alpha,
            this.beta,
            this.normType,
            this.dtype === undefined ? src.type() : this.dtype
          );

          this.sources.push(out);
          this.output(out);
        }
      }
    }
  };
}
